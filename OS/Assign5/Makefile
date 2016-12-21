#Makefile for Eric Wang
#assignment 5

as5: main.h main.o best.o first.o buddy.o
	gcc -Wall -o as5 main.o best.o first.o buddy.o

main.o: main.c
	gcc -c main.c

best.o: best.c
	gcc -c best.c

first.o: first.c
	gcc -c first.c

buddy.o: buddy.c
	gcc -c buddy.c

clean:
	$(RM) *.o
	$(RM) as5