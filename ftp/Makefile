CC=gcc

CFLAGS=-c

all: ftp

ftp: main.o ftp_data.o
	$(CC) main.o ftp_data.o

main.o:
	$(CC) $(CFLAGS) main.c

ftp_data.o: ftp_data.c ftp_data.h
	$(CC) $(FLAGS) ftp_data.c

clean:
	rm -rf *o ftp