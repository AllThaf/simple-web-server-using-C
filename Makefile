CC=gcc
CFLAGS=-I.

all: parse

clean: 
			@rm -rf *.o
			@rm -rf server

parse: main.o parsing.o
				$(CC) -o parse $^

main.o: main.c parsing.h
				$(CC) -c -o main.o main.c

parsing.o: parsing.c parsing.h
					$(CC) -c -o parsing.o parsing.c
