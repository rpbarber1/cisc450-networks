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
    short rcvHeaderBuffer[HEADERRCVBUFSIZE]; //buffer for header always 4 byte
    char *dataBuffer[DATARCVBUFSIZE]; //buffer for data, max 80 byte
    int bytesRcvd, totalBytesRcvd;

    /*header of request
        - requestCount - Count part of header. Gotten from strlen(filename)
        - requestSeqNum - Sequence Number part of header. Always 0 in request.
    */
    short requestHeader[2]; // array of 2 shorts = 4 bytes
    short requestCount;
    short requestSeqNum = 0;


    // Test for correct number of arguments
    if((argc<2)||(argc>3)){
        printf("Usage: %s <Server IP> <filename>\n", argv[0]);
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

    requestCount = strlen(filename); //data bytes in filename
    requestHeader[0] = htons(requestCount);
    requestHeader[1] = htons(requestSeqNum);

    /*Send filename string
        send -header
        send -data (filename)
    */
    if(send(clientSocket, &requestHeader[0], 4, 0)!= 4){
        DieWithError("client send() sent a differnet number of bytes than expected in header");
    }
    if(send(clientSocket, filename, requestCount, 0)!= requestCount){
        DieWithError("client send() sent a differnet number of bytes than expected in data");
    }

    printf("Packet %hi transmitted with %hi data bytes\n", requestSeqNum, requestCount);

    /* Receive the file from the server
        do while loop recv() call until end of transmission
        print received message each time
        print end of transmission recvd 

        print final message

        close
        
    */

}