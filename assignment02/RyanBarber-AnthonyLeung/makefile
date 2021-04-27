CC=gcc
CFLAGS=-Wall

all: client server

OTHER_OBSS = DieWithError.o

client :	client.o	$(OTHER_OBSS)

server :	server.o	$(OTHER_OBSS) -lm

DieWithError.o : DieWithError.c
				$(CC) -c DieWithError.c

client.o: client.c
				$(CC) -c client.c

server.o: server.c 
				$(CC) -c server.c

clean:
		rm -f	client.o DieWithError.o server.o client.exe server.exe client server out.txt
