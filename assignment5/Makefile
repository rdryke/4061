CC=gcc
CFLAGS=-g -c -Wall
LIBS=-lpthread

all: decryption_server decryption_client
	rm *\.o

decryption_server: queue.o csci4061pa4.o
	$(CC) queue.o csci4061pa4.o -o decryption_server $(LIBS)

decryption_client: queue.o client.o
	$(CC) queue.o client.o -o decryption_client

queue.o: queue.c main.h queue.h
	$(CC) $(CFLAGS) queue.c

csci4061pa4.o: csci4061pa4.c main.h csci4061pa4.h
	$(CC) $(CFLAGS) csci4061pa4.c $(LIBS)

client.o: client.c main.h csci4061pa4.h
	$(CC) $(CFLAGS) client.c

clean:
	rm -rf *\.o decryption_server
