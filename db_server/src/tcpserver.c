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
    
	char buffer[BUFFERSIZE];
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
		
		if((childpid = fork()) == 0){
			close(sockfd);

			while(1){
				recv(newSocket, buffer, sizeof(buffer), 0);
				
				if(strcmp(buffer, "exit") == 0){
					printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
					break;
				}
				else
				{
					request_t* request = parse_request(buffer, NULL);
					print_request(request);
					switch(request->request_type) 
					{
						case RT_CREATE: {
							createTable(request);
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
					}
					bzero(buffer, sizeof(buffer));
				}
			}
		}
	}

	close(newSocket);

	return 0;
}