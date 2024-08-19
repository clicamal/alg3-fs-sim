compile: main.c
	gcc -Wall -g -o main main.c

run: main
	./main

all: compile run
