#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "request.h"
#include "db_engine.h"

#define BUFFERSIZE 1024
int listenPort(int port){

	int sockfd, ret;
	struct sockaddr_in serverAddr;

	int newSocket;
	struct sockaddr_in newAddr;

	socklen_t addr_size;
    
	pid_t childpid;
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("Error in connection.\n");
		exit(1);
	}
	printf("Server Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("Error in binding.\n");
		exit(1);
	}
	printf("Bind to port %d\n", port);

	if(listen(sockfd, 10) == 0){
		printf("Listening....\n");
	}else{
		printf("Error in binding.\n");
	}

	char buf2[BUFFERSIZE];

	while(1){
		newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);
		if(newSocket < 0){
			exit(1);
		}
		printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
		
		if ((childpid = fork()) == 0){
			close(sockfd);
			char buffer[BUFFERSIZE];
			bzero(buffer, BUFFERSIZE);
			int connected = 1;
			while (connected) {
				bzero(buffer, sizeof(buffer));
				recv(newSocket, buffer, sizeof(buffer), 0);

				char* errorbuffer;
				request_t* request = parse_request(buffer, &errorbuffer);

				if (request == NULL) {
					errorbuffer[strlen(errorbuffer)] = '\n';
					printf("%s", errorbuffer);
					send(newSocket, errorbuffer, strlen(errorbuffer), 0);
					free(errorbuffer);
					continue;
				}

				print_request(request);

				switch(request->request_type) 
				{
					case RT_CREATE: {
						createTable(request);
					} break;
					case RT_DROP: {
						deleteTable(request);
					} break;
					case RT_TABLES: {
						char* tables = listTables(request);
						send(newSocket, tables, strlen(tables), 0);
						free(tables);
					} break;
					case RT_SCHEMA: {
						char* schema = getSchemaString(request);
						send(newSocket, schema, strlen(schema), 0);
						free(schema);
					} break;
					case RT_INSERT: {
						insertRecord(request);
					} break;
					case RT_SELECT: {
						char* select = selectRecord(request);
						send(newSocket, select, strlen(select), 0);
						free(select);
					} break;
					case RT_QUIT: {
						connected = 0;
					} break;
					default: {
						printf("request %i not supported\n", request->request_type);
					} break;
				}

				destroy_request(request);
			}

			close(newSocket);
			return 0;
		}
	}

	return 0;
}