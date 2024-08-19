compile: main.c
	gcc -Wall -g -o main main.c

run: main
	./main

all: main.c main
	./main
