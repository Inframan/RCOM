CC=gcc

CFLAGS=-c

all: ftp

ftp: ftp.o ftp_data.o
	$(CC) ftp.o ftp_data.o

ftp_data.o: ftp_data.c ftp_data.h
	$(CC) $(FLAGS) ftp_data.c

clean:
	rm -rf *o ftp