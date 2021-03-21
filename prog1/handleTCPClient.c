#include <stdio.h> /* for printf() and fprintf() */
#include <stdio.h> /* for printf() and fprintf() */
#include <unistd.h> /* for close() */

#define RCVBUFSIZE 32 /* Size of receive buffer */

void DieWithError(char *errorMessage);

void HandleTCPClient(int clientSocket){
    char filenameBuffer[RCVBUFSIZE]; //buffer for filename string
    int recvMsgSize;
    
    //Receive filename message from client
    if((recvMsgSize = recv(clientSocket, filenameBuffer, RCVBUFSIZE, 0)) < 0){
        DieWithError("recv() failed");
    }

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
}