#include <stdio.h> /*for printf() and fprintf()*/
#include <sys/socket.h> /*for socket(), connect(), send(), and recv()*/
#include <arpa/inet.h> /*for sockaddr_in and inet_addr()*/
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXPENDING 5 /* Maximum outstanding connection requests */

void DieWithError(char *errorMessage); /* Error handling function */
void HandleTCPClient(int clntSocket);  /* TCP client handling function */

int main(int argc, char *argv[]){
    int serverSocket; // Socket for server
    int clientSocket; // Socket for incoming client
    struct sockaddr_in serverAddress; // Local address
    struct sockaddr_in clientAddress; // Client address
    unsigned short serverPort = 99999; // Server port
    unsigned int clientLength;
    /*
    Test for correct number of arguments.
        - There are no arguments for server side since port is hard coded
    */
    if(argc != 1){
        printf("No arguments");
        exit(1);
    }

    // Create socket for incoming connections
    if((serverSocket = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP)) < 0 ){
        DieWithError("socket () failed");
    }
    serverAddress.sin_family = AF_INET; /* Internet address family */
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
	serverAddress.sin_port = htons(serverPort); /* Local port */
    /* Bind to the local address */
	if(bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0){
	    DieWithError ( "bind () failed");
    }
    // Mark the socket so it will listen for incoming connections
    if(listen(serverSocket, MAXPENDING) < 0){
        DieWithError("listen() failed");
    }

    for(;;){ //run forever

        /* Set the size of the in-out parameter */
        clientLength = sizeof(clientAddress);

        //Wait for a client to connect
        if((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientLength)) < 0){
            DieWithError("accept() failed");
        }
        //clientSocket is connected to a client!
        HandleTCPClient(clientSocket);
    }

}