#include <stdio.h> /*for printf() and fprintf()*/
#include <sys/socket.h> /*for socket(), connect(), send(), and recv()*/
#include <arpa/inet.h> /*for sockaddr_in and inet_addr()*/
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define HEADERRCVBUFSIZE 4 //Size of header buffer
#define DATARCVBUFSIZE 80 /*Size of data buffer*/

void DieWithError(char *errorMessage);

int main(int argc, char *argv[]){
    int clientSocket;
    struct sockaddr_in serverAddress;
    unsigned short serverPort = 99999;
    char *serverIP;
    char *filename;
    char *headerBuffer[HEADERRCVBUFSIZE];
    char *dataBuffer[DATARCVBUFSIZE];
    unsigned int filenameLen;
    int bytesRcvd, totalBytesRcvd;

    // Test for correct number of arguments
    if((argc<2)||(argc>3)){
        printf("Usage: %s <Server IP> <filename>\n"), argv[0];
        exit(1);
    }

    //Set server IP and filename from arguments
    serverIP = argv[1];
    filename = argv[2];

    //Create TCP socket
    if((clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
        DieWithError("socket() failed");
    }

    //Set struct for server address
    serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(serverIP);
	serverAddress.sin_port = htons(serverPort);

    //Connect to listening server 
    if(connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
        DieWithError("connect() failed");
    }

    filenameLen = strlen(filename);

    //Send filename string
    if(send(clientSocket, filename, filenameLen, 0)!= filenameLen){
        DieWithError("send() sent a differnet number of bytes than expected");
    }

    /* Receive the file from the server
        do while loop recv() call until end of transmission
        print received message each time
        print end of transmission recvd 

        print final message

        close
        
    */

}