CC = gcc

all: server.o client.o

server.o: server.c
	$(CC) server.c -o server.o -lpthread

client.o: client.c
	$(CC) client.c -o client.o -lpthread
