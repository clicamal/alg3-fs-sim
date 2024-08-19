compile-debug: main.c
	gcc -Wall -g -o main main.c

compile: main.c
	gcc -o main main.c

run: compile
	./main
