#include <stdio.h> /* for printf() and fprintf() */
#include <unistd.h> /* for close() */

#define DATARCVBUFSIZE 32 /* Size of data buffer, filename no more than 32 char */

void DieWithError(char *errorMessage);

void HandleTCPClient(int clientSocket){
    /*
        headerBuffer[2]
            headerBuffer[0] = Count
            headerBuffer[1] = sequence number
    */
    short headerBuffer[2]; 
    char *filenameBuffer[DATARCVBUFSIZE]; //buffer for filename bytes
    int recvHeaderSize;
    int recvMsgSize;

    
    //Receive header message from client
    if((recvHeaderSize = recv(clientSocket, &headerBuffer[0], 4, 0)) < 0){
        DieWithError("recv() failed");
    }
    //Receive header message from client
    if((recvMsgSize = recv(clientSocket, filenameBuffer, DATARCVBUFSIZE, 0)) < 0){
        DieWithError("recv() failed");
    }
    
    headerBuffer[0] = ntohs(headerBuffer[0]);
    headerBuffer[1] = ntohs(headerBuffer[1]);

    printf("Packet %hi received with %hi data bytes\n", headerBuffer[1], headerBuffer[0]);

    //WORKING THROUGH HERE---------------------------------------
    
    /*
        Need to read file from filename and set the
        'count' to be num of bytes read <= 80.

        Set sequence num doing a loop maybe

        header bytes in buffer
        data bytes in buffer

        send header, send data
        print sent message
        increment loop, read next bytes
        
        send EOT packet
        print sent message

        print final message
        close
    */

   close(clientSocket);

}