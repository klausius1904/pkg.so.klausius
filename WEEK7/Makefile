CC = gcc
CFLAGS = -Wall

all: prog

prog: main.o messagetype.o systemcalls.o
	$(CC) main.o messagetype.o systemcalls.o -o prog

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

messagetype.o: messagetype.c
	$(CC) $(CFLAGS) -c messagetype.c

systemcalls.o: systemcalls.c
	$(CC) $(CFLAGS) -c systemcalls.c

clean:
	rm -rf *.o prog

