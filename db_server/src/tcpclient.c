#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 

int main(int argc, char *argv[])
{
    int portNumber = 8080;

	//Create a socket
    int network_socket;
    //0 meaning we are using default protocol
    network_socket = socket(AF_INET,SOCK_STREAM,0);
    
    // specify an adddress for the socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET; //set type of address
    server_address.sin_port = htons(portNumber);
    server_address.sin_addr.s_addr = INADDR_ANY;//connects to 0.0.0, 

    int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    // check for error with the connection
    if(connection_status == -1)
    {
        printf("There was an error making a connection to remove socket \n\n");
    }
    //recive data from the server
    char server_response[256];
    recv(network_socket, &server_response, sizeof(server_response), 0);

    //print out the server's reponse
    printf("The server sent the data : %s",server_response);
    // and then close the socket;
    close(sock);
    return 0;
}
