#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFERSIZE 1024

int main(){

	int clientSocket, ret;
	struct sockaddr_in serverAddr;
	char buffer[BUFFERSIZE];
	memset(buffer, 0, sizeof(buffer));

	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocket < 0){
		printf("Error in connection.\n");
		exit(1);
	}
	printf("Client Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("Error in connection.\n");
		exit(1);
	}
	printf("Connected to Server %d\n", PORT);

	while(1)
	{
				
		printf("Client: ");
		scanf("%s",&buffer[0]);
		send(clientSocket,buffer,strlen(buffer), 0);		
		if(strcmp(buffer,"exit")==0){
			close(clientSocket);
			printf("Disconnect from server");
			exit(1);
		}
		memset(buffer, 0, sizeof(buffer));

	
		if(recv(clientSocket,buffer,BUFFERSIZE,0) < 0)
		{
			printf("Error in receiving data");
		}
		else{
			printf("Server: %s\n",buffer);
		}
	
	}

	return 0;
}