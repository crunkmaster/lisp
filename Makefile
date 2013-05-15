# Makefile for LISP Interpreter
# 
#

CC = gcc
CFLAGS = -g -ansi -pedantic -Wall

lisp: main.o interpreter.o clist.o lex.o
	$(CC) $(CFLAGS) interpreter.o clist.o lex.o main.o -o lisp

main.o: interpreter.h globals.h
	$(CC) $(CFLAGS) -c main.c

clist.o: clist.h globals.h
	$(CC) $(CFLAGS) -c clist.c

interpreter.o: lex.h interpreter.h clist.h globals.h
	$(CC) $(CFLAGS) -c interpreter.c

lex.o: lex.h
	$(CC) $(CFLAGS) -c lex.c

clean:
	rm -f *.o
	rm -f lisp
