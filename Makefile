all: hello

hello: client.o server.o
	

client.o: 
	gcc client.c -o client
server.o: 
	gcc server.c -o server
