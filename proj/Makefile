CC=gcc

CFLAGS=-c -w

all: download

download: main.o ftp_data.o utils.o config.o test.o message.o ftp.o
	$(CC) main.o ftp_data.o utils.o config.o test.o message.o ftp.o -o download

main.o: main.c
	$(CC) $(CFLAGS) main.c

ftp_data.o: ftp_data.c ftp_data.h
	$(CC) $(CFLAGS) ftp_data.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) utils.c

config.o: config.c config.h
	$(CC) $(CFLAGS) config.c

test.o: test.c test.h
	$(CC) $(CFLAGS) test.c

message.o: message.c message.h
	$(CC) $(CFLAGS) message.c

ftp.o: ftp.c ftp.h
	$(CC) $(CFLAGS) ftp.c

clean:
	rm -rf *o download
