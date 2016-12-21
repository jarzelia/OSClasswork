#Makefile for Eric Wang
#assignment 3

proddonuts consdonuts: donuts.h proddonuts.o consdonuts.o utilities.o
	gcc -o proddonuts proddonuts.o utilities.o
	gcc -o consdonuts consdonuts.o utilities.o
consdonuts.o: donuts.h consdonuts.c
	gcc -c consdonuts.c
proddonuts.o: donuts.h proddonuts.c
	gcc -c proddonuts.c
utilities.o:  utilities.c
	gcc -c utilities.c
clean:
	$(RM) *.o 
	$(RM) consdonuts
	$(RM) proddonuts