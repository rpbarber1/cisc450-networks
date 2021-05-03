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
#include <math.h>
#include <errno.h>

#define MESSAGEBUFFER 84 // max num of bytes in line, also restricts file name request
#define DATABUFFER 80//max size of message (data part)

void DieWithError(char *errorMessage); /* Error handling function */
void sendMessage(int serversock, char *filename, double loss_ratio, struct sockaddr_in *clntaddr);
int SimulateLoss(double loss_ratio);

//Message struct
struct msg{
    short count; // count - Count part of header. Gotten from strlen(filename)
    short seqnum; // seqNum - Sequence Number part of header. Always 0 in request
    char message[DATABUFFER]; // message - data part of message
};

struct ack{
    short seqnum;  // only sequence number in ack packet
};



int SimulateLoss(double loss_ratio){
	double r = ((double) rand() / (RAND_MAX));
	
	if(r < loss_ratio){
		return 1;
	} else {
		return 0;
	}
}
//============================================================================




void sendMessage(int serversock, char *filename, double loss_ratio, struct sockaddr_in *clntaddr){
    struct msg messageToSend;
    short currentSeqNum = 0;
    short currentBytesSent = 0;
    int recvMsgSize;
    
    struct ack messageToReceive;
    
    //statistic values
    int trans_generated = 0;
    int retrans_generated = 0;
    int trans_successful = 0;
    int totalbytes = 0;
    int num_dropped = 0;
    int num_ack = 0;
    int num_timeouts = 0;
    
    int is_timeout = 0;


    //open file stream for requested file
    FILE *fp = fopen(filename, "r");
    //loop reading file until 
    //fgets = 0 when EOF or error, check at end

    while(fgets(messageToSend.message, DATABUFFER, fp) > 0){
        //strlen reads \n not \0
        currentBytesSent = strlen(messageToSend.message); 
        totalbytes += currentBytesSent;

        messageToSend.count = htons(currentBytesSent);
        messageToSend.seqnum = htons(currentSeqNum);
        
        printf("Packet %hi generated for transmission with %hi data bytes\n", currentSeqNum,currentBytesSent);
        trans_generated ++;
        
	if(SimulateLoss(loss_ratio) == 0){
        	//send whole message
        	if(sendto(serversock, &messageToSend, (currentBytesSent+4), 0, (struct sockaddr *) clntaddr, 
        		sizeof(*clntaddr)) != currentBytesSent+4){
        	
            		DieWithError("send() failed");
        	}

        	printf("Packet %hi successfully transmitted with %hi data bytes\n", currentSeqNum,currentBytesSent);
        	trans_successful ++;
        
	} else {
		printf("Packet %hi lost\n", currentSeqNum);
		num_dropped ++;
	}
	
	// START TIMER
	// STOP AND WAIT FOR ACK 
	
	/* Set the size of the in-out parameter */
        unsigned int clientLength = sizeof(*clntaddr);
        
        // Recieve message 
        if((recvMsgSize = recvfrom(serversock, &messageToReceive, MESSAGEBUFFER, 0, (struct sockaddr *) clntaddr, &clientLength)) < 0){
            if(errno == EAGAIN || errno == EWOULDBLOCK){
                printf("Timeout  expired for Packet with sequence number %hi\n", currentSeqNum);
                is_timeout = 1;
            	num_timeouts ++;
            	//continue;
            } else {
            	DieWithError("recv() failed");
            }
        }
        
        //if not timeout, set messagetoreceive seqnum
        if(is_timeout == 0){
        	// convert header data order
        	messageToReceive.seqnum = ntohs(messageToReceive.seqnum);
        
        	printf("ACK %hi received\n", messageToReceive.seqnum);
        	num_ack ++;
        } else {
        	//if timeout, set seqnum to 2 (not valid) to go into while loop
        	messageToReceive.seqnum = 2;
        }
        
        // if seq nums don't match, retransmit and wait for ack until seq nums match
        //there is no limit on number of timeouts. it will keep retransmitting until ack rcvd
        while(currentSeqNum != messageToReceive.seqnum){

        	printf("Packet %hi generated for re-transmission with %hi data bytes\n", currentSeqNum,currentBytesSent);
        	retrans_generated ++;
        	
        	// simulate loss again and send whole message
        	if(SimulateLoss(loss_ratio) == 0){
        		if(sendto(serversock, &messageToSend, (currentBytesSent+4), 0, (struct sockaddr *) clntaddr, 
        			sizeof(*clntaddr)) != (currentBytesSent+4)){
        	
            			DieWithError("send() failed");
        		}
        		printf("Packet %hi successfully transmitted with %hi data bytes\n",currentSeqNum,currentBytesSent);
        		trans_successful ++;
        
		} else {
			printf("Packet %hi lost\n", currentSeqNum);
			num_dropped ++;
		}
		
		/* Set the size of the in-out parameter */
        	unsigned int clientLength = sizeof(*clntaddr);
        	
        	//reset timeout bool
        	is_timeout = 0;
        	
        
        	// Recieve message 
        	if((recvMsgSize = recvfrom(serversock, &messageToReceive, MESSAGEBUFFER, 0, (struct sockaddr *) clntaddr, &clientLength)) < 0){
            		if(errno == EAGAIN || errno == EWOULDBLOCK){
            		        printf("Timeout  expired for Packet with sequence number %hi\n", currentSeqNum);
                		is_timeout = 1;
            			num_timeouts ++;
            			//continue;
           		} else {
            			DieWithError("recv() failed");
            		}
        	}
        	
        	//if not timeout, set messagetoreceive seqnum
        	if(is_timeout == 0){
        		// convert header data order
        		messageToReceive.seqnum = ntohs(messageToReceive.seqnum);
        
        		printf("ACK %hi received\n", messageToReceive.seqnum);
        		num_ack ++;
        	} else {
        		//if timeout, set seqnum to 2 (not valid) to go into while loop again
        		messageToReceive.seqnum = 2;
        	}
        } 

	
	if(currentSeqNum == 0){
        	currentSeqNum = 1;
        } else {
        	currentSeqNum = 0;
        }
	
    }

    if(ferror(fp)){
        DieWithError("fgets() read failed");
    }

    // Create EOT packet
    messageToSend.count = htons(0);
    messageToSend.seqnum = htons(currentSeqNum);
    messageToSend.message[0] = '\0';

    // send EOT packet
    // hard coded EOT packet number of bytes = 5
    if(sendto(serversock, &messageToSend, 5, 0, (struct sockaddr *) clntaddr, sizeof(*clntaddr)) != 5){
        DieWithError("send() failed");
    }

    printf("End of Transmission Packet with sequence number %hi transmitted\n", currentSeqNum);

    fclose(fp);
    
    //statistics
    printf("\nNumber of data packets generated for transmission: %d\n", trans_generated);
    printf("Total number of data bytes generated for transmission: %d\n", totalbytes);
    printf("Total number of data packets generated for retransmission: %d\n", retrans_generated+trans_generated);
    printf("Number of data packets dropped due to loss: %d\n", num_dropped);
    printf("Number of data packets transmitted successfully: %d\n", trans_successful);
    printf("Number of ACKs received: %d\n", num_ack);
    printf("Count of how many times timeout expired: %d\n\n", num_timeouts); 
    
}

//=================================================================================

int main(int argc, char *argv[]){
    int sock; // Socket
    struct sockaddr_in serverAddress; // Local address
    struct sockaddr_in clientAddress; // Client address
    unsigned short serverPort = 10000; // Server port
    unsigned int clientLength;
    int recvMsgSize; // total size of struct = header + data

    struct msg messageToReceive; //struct for incoming message
    
    int timeout;
    double loss_ratio;
    double microsec;

    struct timeval tv; //for timer
    
    // seed random
    srand(time(NULL));

    /*
    Test for correct number of arguments.
        - 3 args - ./server [timeout value] [loss ratio]
        timeout value - between 1 and 10
        loss ratio - between 0 and 1
    */
    if(argc != 3){
        printf("Usage: %s [timeout value] [packet loss ratio]", argv[0]);
        exit(1);
    }
    
    //set values from args
    timeout = atoi(argv[1]);
    loss_ratio = atof(argv[2]);
    microsec = pow((double)timeout, -6.0);
    tv.tv_sec = microsec;
    tv.tv_usec = timeout;
    

    // Create socket for incoming connections
    if((sock = socket(PF_INET, SOCK_DGRAM,IPPROTO_UDP)) < 0 ){
        DieWithError("socket () failed");
    }
    
    /* Construct local address structure */
    memset(&serverAddress, 0, sizeof(serverAddress)); /* Zero out structure */
    serverAddress.sin_family = AF_INET; /* Internet address family */
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    serverAddress.sin_port = htons(serverPort); /* Local port */
	
    /* Bind to the local address */
    if(bind(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0){
        DieWithError ( "bind () failed");
    }
    
    
    for(;;){ //run forever

        /* Set the size of the in-out parameter */
        clientLength = sizeof(clientAddress);
        
        // Recieve message
        if((recvMsgSize = recvfrom(sock, &messageToReceive, MESSAGEBUFFER, 0, (struct sockaddr *) &clientAddress,&clientLength)) < 0){
            DieWithError("recv() failed");
        }
        
        // convert header data order
        messageToReceive.count = ntohs(messageToReceive.count);
        messageToReceive.seqnum = ntohs(messageToReceive.seqnum);
        
        //send file and wait for acks
        //set timeout for send message 
        tv.tv_sec = microsec;
    	tv.tv_usec = timeout;
    	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    	
        sendMessage(sock, messageToReceive.message, loss_ratio, &clientAddress);
        
        //disable timer for next run in this loop; do not time out while waiting for initial request
        tv.tv_sec = 0;
    	tv.tv_usec = 0;
    	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
        
    }

}
