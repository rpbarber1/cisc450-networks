/*
	Ryan Barber
	Anthony Leung
	CISC 450
	Program 1
*/


#include <stdio.h> /*for printf() and fprintf()*/
#include <sys/socket.h> /*for socket(), connect(), send(), and recv()*/
#include <arpa/inet.h> /*for sockaddr_in and inet_addr()*/
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define HEADERBUFFER 4 //Size of header buffer
#define READBUFFER 80 // Max num of chars in data part of packet.

void DieWithError(char *errorMessage);

//Message struct
struct msg{
    short count; // count - Count part of header. Gotten from strlen(filename)
    short seqnum; // seqNum - Sequence Number part of header. Always 0 in request
    char message[READBUFFER]; // message - data part of message
};

int main(int argc, char *argv[]){
    int clientSocket;
    struct sockaddr_in serverAddress;
    unsigned short serverPort = 10000;
    char *serverIP;
    int endTransmission = 1; //boolean for reveive loop

    int dataBufferSize;
    int sequenceNum = 0;
    struct msg messageToSend; //message struct for request packet
    
    // Test for correct number of arguments
    if((argc<2)||(argc>3)){
        printf("Usage: %s <Server IP> <filename>\n", argv[0]);
        exit(1);
    }

    //Set server IP and filename from arguments
    serverIP = argv[1];
    strcpy(&messageToSend.message, argv[2]);

    //set header fields of message struct
    dataBufferSize = strlen(messageToSend.message);

    messageToSend.count = htons(dataBufferSize);
    messageToSend.seqnum = htons(sequenceNum);

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

    /*Send filename string
        send -header
        send -data (filename)
    */
    if(send(clientSocket, &messageToSend, HEADERBUFFER, 0)!= 4){
        DieWithError("client send() sent a differnet number of bytes than expected in header");
    }
    if(send(clientSocket, &messageToSend.message, dataBufferSize, 0)!= dataBufferSize){
        DieWithError("client send() sent a differnet number of bytes than expected in data");
    }

    printf("Packet %hi transmitted with %hi data bytes\n", sequenceNum, dataBufferSize);

    // Receive the file from the server

    short currenBytesRcvd, totalBytesRcvd = 0;
    short currentSeqNum;

    struct msg messageToRecv;

    FILE *fp = fopen("out.txt", "w");

    while(endTransmission){
        if(currenBytesRcvd = recv(clientSocket, &messageToRecv, HEADERBUFFER, 0) <= 0){
            DieWithError("recv() failed or connection closed prematurely");
        }
        messageToRecv.seqnum = ntohs(messageToRecv.seqnum);
        messageToRecv.count = ntohs(messageToRecv.count);
        totalBytesRcvd = totalBytesRcvd + messageToRecv.count;

        memset(&messageToRecv.message, 0, READBUFFER);
        if(currenBytesRcvd = recv(clientSocket, &messageToRecv.message, messageToRecv.count, 0) < 0){
            DieWithError("recv() failed or connection closed prematurely");
        }

        if(messageToRecv.count == 0){
            endTransmission = 0;
            printf("End of Transmission Packet with sequence number %hi received with %hi data bytes\n", messageToRecv.seqnum, messageToRecv.count);
            break;
        }

        printf("Packet %hi, received with %hi data bytes\n", messageToRecv.seqnum, messageToRecv.count);

        //write buffer to file

        if(fputs(&messageToRecv.message, fp) < 0){
            DieWithError("fputs() write failed");
        }

    }

    close(fp);
    close(clientSocket);

    printf("Number of data packets received: %hi\nTotal number of data bytes received: %hi\n", messageToRecv.seqnum-1, totalBytesRcvd);

}
