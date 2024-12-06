CC=gcc
CFLAGS=-I.

all: server

clean: 
			@rm -rf *.o
			@rm -rf server

server: main.o server.o file.o
				$(CC) -o server $^

main.o: main.c server.h
				$(CC) -c -o main.o main.c

server.o: server.c server.h file.h
					$(CC) -c -o server.o server.c

file.o: file.c file.h
				$(CC) -c -o file.o file.c
