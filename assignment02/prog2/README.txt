Ryan Barber
Anthony Leung
CISC 450
Programming Assignment 2


The code in this project uses most of the same code from the 'example-code' folder UDPEcho

List of Relevant Files:
	server.c
	client.c
	DieWithError.c
	makefile

Compilation Instructions
	Complie:
		make all
	Clean:
		make clean

	Doing the 'make clean' command will remove the ouput file with the object and executables.

Execution Instructions (bash shell) - I use Linux Ubuntu 20.04
	Server side:
		./server [timeout interval] [packet loss rate]
		
		example: Packet Loss rate = 0.2, Timeout = 5
			./server 5 0.2

	Client side:
		./client 127.0.0.1 [filename] [ACK loss rate]
		
		example: ACK loss rate = 0.2, filename = input-file.txt
			./client 127.0.0.1 input-file.txt 0.2

Running Instructions
	First, open 2 terminal windows. Then execute the server program first (there is no output).
	Next, in the other terminal window, execute the client program. Once complete, kill the 
	server program with ctrl+C or run client program again.

