# Makefile for LISP Interpreter
# 
#

CC = gcc
CFLAGS = -g -ansi -pedantic -Wall

all: lisp testdriver

lisp: main.o interpreter.o clist.o lex.o clist_interface.o
	$(CC) $(CFLAGS) interpreter.o clist.o lex.o main.o clist_interface.o -o lisp

testdriver: testdriver.o clist_interface.o clist.o interpreter.o lex.o
	$(CC) $(CFLAGS) -o testdriver testdriver.o clist_interface.o interpreter.o clist.o lex.o

testdriver.o: clist_interface.h interpreter.h 
	$(CC) $(CFLAGS) -c testdriver.c

main.o: interpreter.h globals.h
	$(CC) $(CFLAGS) -c main.c

clist_interface.o: clist_interface.h clist_interface.c globals.h
	$(CC) $(CFLAGS) -c clist_interface.c

clist.o: clist.h globals.h
	$(CC) $(CFLAGS) -c clist.c

interpreter.o: lex.h interpreter.h clist.h globals.h clist_interface.h
	$(CC) $(CFLAGS) -c interpreter.c

lex.o: lex.h
	$(CC) $(CFLAGS) -c lex.c

clean:
	rm -f *.o
	rm -f lisp
	rm -f testdriver
