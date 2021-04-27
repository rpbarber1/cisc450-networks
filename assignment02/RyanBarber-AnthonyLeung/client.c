/*
	Ryan Barber
	Anthony Leung
	CISC 450
	Programming Assignment 2

*/

#include <stdio.h> /*for printf() and fprintf()*/
#include <sys/socket.h> /*for socket(), connect(), send(), and recv()*/
#include <arpa/inet.h> /*for sockaddr_in and inet_addr()*/
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#define MESSAGEBUFFER 84 // Max size of packet

void DieWithError(char *errorMessage);
int SimulateACKLoss(double loss_ratio);

//Message struct
struct msg{
    short count; // count - Count part of header. Gotten from strlen(filename)
    short seqnum; // seqNum - Sequence Number part of header. Always 0 in request
    char message[80]; // message - data part of message
};

struct ack{
    short seqnum;
};

//loss function
int SimulateACKLoss(double loss_ratio){
	double r = ((double) rand() / (RAND_MAX));
	if(r < loss_ratio){
		return 1;
	} else {
		return 0;
	}
}

int main(int argc, char *argv[]){
    int clientSocket;
    struct sockaddr_in serverAddress; //Server address
    struct sockaddr_in clientAddress; //Source (Client) address
    unsigned int clientAddressSize;
    unsigned short serverPort = 10000;
    char *serverIP;
    int endTransmission = 1; //boolean for reveive loop

    int dataBufferSize;
    int sequenceNum = 0;
    struct msg messageToSend; //message struct for request packet
    
 
    double loss_ratio;
    
    //seed random
    srand(time(NULL));
    
    // Test for correct number of arguments
    if(argc != 4){
        printf("Usage: %s [Server IP] [filename] [packet loss ratio]\n", argv[0]);
        exit(1);
    }

    //Set server IP and filename from arguments
    serverIP = argv[1];
    strcpy(messageToSend.message, argv[2]);
    
    loss_ratio = atof(argv[3]);

    //set header fields of message struct
    dataBufferSize = strlen(messageToSend.message);

    messageToSend.count = htons(dataBufferSize);
    messageToSend.seqnum = htons(sequenceNum);

    //Create UDP socket
    if((clientSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
        DieWithError("socket() failed");
    }

    //Set struct for server address
    memset(&serverAddress, 0, sizeof(serverAddress)); /* Zero out structure */
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(serverIP);
    serverAddress.sin_port = htons(serverPort);

    /*Send message for file request */
    if(sendto(clientSocket, &messageToSend, dataBufferSize+4, 0, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) != dataBufferSize+4){
        DieWithError("client send() sent a differnet number of bytes than expected in data");
    }

    // Receive the file from the server and send acks

    // statistics
    short currenBytesRcvd, totalBytesRcvd = 0;
    short currentSeqNum = 2;
    int rcvd_packets = 0;
    int dups_rcvd = 0;
    int total_rcvd = 0;
    int ack_sent = 0;
    int ack_dropped = 0;
    int ack_generated = 0;

    struct msg messageToRecv;
    struct ack ackToSend;

    FILE *fp = fopen("out.txt", "w");

    while(endTransmission){
    
    	
    	//receive data packet from server
        memset(messageToRecv.message, 0, 80);
        if(currenBytesRcvd = recvfrom(clientSocket, &messageToRecv, MESSAGEBUFFER, 0, (struct sockaddr *) &clientAddress, &clientAddressSize) < 0){
            DieWithError("recvfrom() failed ");
        }
        
        if (serverAddress.sin_addr.s_addr != clientAddress.sin_addr.s_addr)
	{
		fprintf(stderr,"Error: received a packet from unknown source.\n");
		exit(1);
	}
	
        messageToRecv.seqnum = ntohs(messageToRecv.seqnum);
        messageToRecv.count = ntohs(messageToRecv.count);
        
	// Check for EOT packet
        if(messageToRecv.count == 0){
            endTransmission = 0;
            printf("End of Transmission Packet with sequence number %hi received\n", messageToRecv.seqnum);
            break;
        }
        
        //check for duplicates
        if(currentSeqNum == messageToRecv.seqnum){
        	printf("Duplicate packet %hi received with %hi data bytes\n", currentSeqNum, messageToRecv.count);
        	dups_rcvd ++;
        } else {
        // normal packet received, write to file
        	printf("Packet %hi received with %hi data bytes\n", messageToRecv.seqnum, messageToRecv.count);
        	rcvd_packets ++;
        	totalBytesRcvd = totalBytesRcvd + messageToRecv.count;
        	//write buffer to file

        	if(fputs(messageToRecv.message, fp) < 0){
            		DieWithError("fputs() write failed");
        	}
        
        	printf("Packet %hi delivered to user\n", messageToRecv.seqnum);
        }
        
        //create ack response
        currentSeqNum = messageToRecv.seqnum;
        ackToSend.seqnum = htons(messageToRecv.seqnum);
        
        printf("ACK %hi generated for transmission\n", currentSeqNum);
        ack_generated ++;
        
        //send ack
        if(SimulateACKLoss(loss_ratio) == 0){
        	if(sendto(clientSocket, &ackToSend, 2, 0, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) != 2){
        		DieWithError("send() sent a different number of bytes than expected");
        	}
        	printf("ACK %hi successfully transmitted\n", currentSeqNum);
        	ack_sent ++;
        
        } else {
        	printf("ACK %hi lost\n", currentSeqNum);
        	ack_dropped ++;
        }
        

    }

    fclose(fp);
    close(clientSocket);

    //statistics
    printf("\nTotal number of data packets received successfully: %d\n", rcvd_packets + dups_rcvd);
    printf("Number of duplicate data packets received: %d\n", dups_rcvd);
    printf("Number of data packets received successfully, not including duplicates: %d\n", rcvd_packets);
    printf("Total number of data bytes received which are delivered to user: %d\n", totalBytesRcvd);
    printf("Number of ACKs transmitted without loss: %d\n", ack_sent);
    printf("Number of ACKs generated but dropped due to loss: %d\n", ack_dropped);
    printf("Total number of ACKs generated: %d\n\n", ack_generated);
    

}
