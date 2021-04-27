CC=gcc
CFLAGS=-Wall

all: client server

OTHER_OBSS = DieWithError.o

client :	client.o	$(OTHER_OBSS)

server :	server.o	$(OTHER_OBSS)

DieWithError.o : DieWithError.c
				$(CC) -c DieWithError.c

client.o: client.c
				$(CC) -c client.c

sever.o: sever.c 
				$(CC) -c sever.c

clean:
		rm -f	client.o DieWithError.o server.o client.exe sever.exe client server out.txt
