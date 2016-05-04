
all: cbtimer.o main.o

cbtimer.o:
	gcc -C cbtimer.c -o cbtimer.o 

main.o:
	gcc -C main.c -o main.o



