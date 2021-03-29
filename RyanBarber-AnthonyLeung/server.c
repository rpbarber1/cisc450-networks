/*
	Ryan Barber
	Anthony Leung
	CISC 450
	Program 1

	File contains fucntions for creating socket,
	sending and receving messages over sockets,
	and reading file for transmission.
*/


#include <stdio.h> /*for printf() and fprintf()*/
#include <sys/socket.h> /*for socket(), connect(), send(), and recv()*/
#include <arpa/inet.h> /*for sockaddr_in and inet_addr()*/
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXPENDING 5 /* Maximum outstanding connection requests */
#define HEADERBUFFER 4 // header is always 4 bytes
#define DATABUFFER 80 // max num of bytes in line, also restricts file name request

void DieWithError(char *errorMessage); /* Error handling function */
void sendMessage(int clntSocket, char *filename);  /* TCP client handling function */

//Message struct
struct msg{
    short count; // count - Count part of header. Gotten from strlen(filename)
    short seqnum; // seqNum - Sequence Number part of header. Always 0 in request
    char message[DATABUFFER]; // message - data part of message
};

/*
	Read file and send over socket
	- clntSocket - desination socket
	- filename - name of file to be transmitted
*/
void sendMessage(int clntSocket, char *filename){
    struct msg messageToSend;
    short currentSeqNum = 1;
    short currentBytesSent, totalBytesSent = 0;


    //open file stream for requested file

    FILE *fp = fopen(filename, "r"); //file pointer for reading input file
    
    //loop reading file until 
    //fgets = 0 when EOF or error, check at end
    while(fgets(&messageToSend.message, DATABUFFER+1, fp) > 0){
        //strlen reads \n not \0
        currentBytesSent = strlen(&messageToSend.message); 
        totalBytesSent = totalBytesSent + currentBytesSent;

        messageToSend.count = htons(currentBytesSent);
        messageToSend.seqnum = htons(currentSeqNum);
        //send header
        if(send(clntSocket, &messageToSend, HEADERBUFFER, 0) != HEADERBUFFER){
            DieWithError("send() failed");
        }

        //send data
        //sending 80 bytes
        if(send(clntSocket, &messageToSend.message, currentBytesSent, 0) != currentBytesSent){
            DieWithError("send() failed");
        }

        printf("Packet %hi transmitted with %hi data bytes\n", currentSeqNum,currentBytesSent);
        
        currentSeqNum ++;

    }

    //check if fgets() is done or there was an error
    if(ferror(fp)){
        DieWithError("fgets() read failed");
    }

    // Create EOT packet
    messageToSend.count = htons(0);
    messageToSend.seqnum = htons(currentSeqNum);
    messageToSend.message[0] = "\0";

    // send EOT packet
    if(send(clntSocket, &messageToSend, HEADERBUFFER, 0) != HEADERBUFFER){
        DieWithError("send() failed");
    }

    if(send(clntSocket, &messageToSend.message, 1, 0) != 1){
        DieWithError("send() failed");
    }

    printf("End of Transmission Packet with sequence number %hi transmitted with %hi data bytes\n", currentSeqNum, messageToSend.count);

    fclose(fp);
    close(clntSocket);
    
    printf("Number of data packets transmitted: %hi\nTotal number of data bytes transmitted: %hi\n", currentSeqNum-1, totalBytesSent);
}

//=================================================================================

int main(int argc, char *argv[]){
    int serverSocket; // Socket for server
    int clientSocket; // Socket for incoming client
    struct sockaddr_in serverAddress; // Local address
    struct sockaddr_in clientAddress; // Client address
    unsigned short serverPort = 10000; // Server port
    unsigned int clientLength;
    int recvHeaderSize, recvMsgSize;

    struct msg messageToReceive; //struct for incoming message

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
        
        // Recieve message and send args to handleTCPClient (clientsocket , filename)
        //Receive header message from client
        if((recvHeaderSize = recv(clientSocket, &messageToReceive, HEADERBUFFER, 0)) < 0){
            DieWithError("recv() failed");
        }
        messageToReceive.count = ntohs(messageToReceive.count);
        messageToReceive.seqnum = ntohs(messageToReceive.seqnum);

        //Receive data message from client
        memset(&messageToReceive.message, 0,DATABUFFER);
        if((recvMsgSize = recv(clientSocket, &messageToReceive.message, messageToReceive.count, 0)) < 0){
            DieWithError("recv() here failed");
        }

        printf("Packet %hi received with %hi data bytes\n", messageToReceive.seqnum, messageToReceive.count);
        sendMessage(clientSocket, &messageToReceive.message);


    }

}
