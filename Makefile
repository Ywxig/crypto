all:
	g++ main.c -o main

run: all
	./main
