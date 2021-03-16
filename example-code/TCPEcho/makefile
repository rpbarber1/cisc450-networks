CC=gcc
CFLAGS=-Wall

all: TCPEchoClient TCPEchoServer

OTHER_OBSS = DieWithError.o HandleTCPClient.o

TCPEchoClient :	TCPEchoClient.o	$(OTHER_OBSS)

TCPEchoServer :	TCPEchoServer.o	$(OTHER_OBSS)

DieWithError.o : DieWithError.c
				gcc -c DieWithError.c

HandleTCPClient.o : HandleTCPClient.c
				gcc -c HandleTCPClient.c

TCPEchoClient.o: TCPEchoClient.c
				gcc -c TCPEchoClient.c

TCPEchoSever.o: TCPEchoSever.c 
				gcc -c TCPEchoSever.c

clean:
		rm -f	TCPEchoClient.o	DieWithError.o HandleTCPClient.o TCPEchoServer.o TCPEchoClient.exe TCPEchoSever.exe

				
