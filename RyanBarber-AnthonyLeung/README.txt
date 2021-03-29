Ryan Barber
Anthony Leung

The code in our project uses most of the same code from the 'example-code' folder

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

Execution Instructions (bash shell)
	Server side:
		./server
	Client side:
		For test file provided:		./client 127.0.0.1 input-file.txt
		For generic input file:		./client 127.0.0.1 <filename> 

Running Instructions
	First, open 2 terminal windows. Then execute the server program first (there is no output).
	Next, in the other terminal window, execute the client program. Once complete, kill the 
	server program with ctrl+C or run client program again.

