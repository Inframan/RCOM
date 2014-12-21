#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <errno.h>
#include <netdb.h> 
#include <sys/socket.h>
#include <sys/types.h>
#include <resolv.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <unistd.h>

#include "defines.h"
#include "utils.h"
#include "ftp_data.h"

/**
@brief Tests if there is an error code (http://en.wikipedia.org/wiki/List_of_FTP_server_return_codes) in the message
@note Only some errors were taken into consideration, since there are a lot of them
@param message - message string
@return Returns OK (0) if success
*/

int ftp_valid(char *message){

	if(strstr(message, FTP_CODE_NO_FILE) != NULL)
		exit(ABORT);
	else if(strstr(message, FTP_CODE_WRONG_CREDENTIALS) != NULL)
		exit(ABORT);
	else
		return OK;

}

/**
@brief Tests if it could receive anything
@param sockfd - socket file descriptor
@param str - buffer string
@return Returns OK (0) if success
*/

int test_receive(int sockfd, char *str){

	if(recv(sockfd, str, MAX_SIZE, ZERO) < ZERO){
		printf("[ERROR]: %s", strerror(errno));
		return !OK;
	}

	return OK;

}

/**
@brief Checks if the only argument that exists is the FTP request
@param argc - number of parameters
@param argv - parameters value
@return Returns OK (0) if success
*/

int test_args(int argc, char *argv[]){

	if (argc != TWO) {
		printf("Program: ./download ftp://[<user>:<password>@]<host>[:port]/<url-path>\n");
		exit(ABORT);
	} else if(argv[ONE] == NULL){
		printf("[ERROR]: Nothing was indicated\n");
		exit(ABORT);
	} else if(strncmp(argv[ONE], FTP_START, SIX) != ZERO){
		printf("[ERROR]: It should start by ftp://\n");
		exit(ABORT);
	}

	return OK;
}

/**
@brief Establishes the connection based on hostname	
@param hostname - hostname
@return Returns the IP address if succeeded
*/

char* getIP(char *hostname) {

	struct hostent *h;

	if ((h = gethostbyname(hostname)) == NULL) {
		herror("gethostbyname");
		exit(ABORT);
	}

	printf("Hostname: %s\n", h->h_name);

	char *ip = inet_ntoa(*((struct in_addr *)h->h_addr));

	printf("\nIP Address : %s\n\n", inet_ntoa(*((struct in_addr *)h->h_addr)));
	
	return ip;
}

/**	
@brief Tries to assign the socket file descriptor (asocket -> assign socket)
@param ip - IP address that comes from getIP function
@param port - port that comes from parsing
@return sockfd if success
*/

int asocket(char *ip, int port){
	
	int sockfd;
	struct sockaddr_in server_addr;
	struct in_addr address;

	bzero((char*) &server_addr, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);                /*server TCP port must be network byte ordered */

	if ((sockfd = socket(AF_INET, SOCK_STREAM, ZERO)) < ZERO){
		printf("[ERROR]: Socket could not be created\n");
		exit(ABORT);
	}
	
	if(inet_aton(ip, &address) == ZERO) {
		printf("[ERROR]: Failure getting an IP\n");
		exit(ABORT);
	}

	server_addr.sin_addr.s_addr = address.s_addr;
	
	if(connect(sockfd, (struct sockaddr*) &server_addr, sizeof (server_addr)) != ZERO){
		printf("[ERROR]: Failure connecting to hport\n");
		exit(ABORT);
	}

	return sockfd;
}

/**
@brief Listens to any message that comes from server
@param sockfd - socket file descriptor
@param str - string to get read message
@return Returns 0 if success
*/

int listen_to(int sockfd, char *str){

	bzero(str, MAX_SIZE);
	
	test_receive(sockfd, str);

	printf("%s\n", str);

	ftp_valid(str);

	return OK;
}

/**
@brief Sends info to server (USER | PASS | etc...)
@param sockfd - socket file descriptor
@param str - string to be sent
@param code_str - USER | PASS | PASV | etc...
@param value - value
@return Returns 0 if success
*/

int send_to(int sockfd, char *str, char *code_str, char *value){

	bzero(str, MAX_SIZE);

	strcpy(str, code_str);
	strcat(str, value);
	strcat(str, NEWLINE_STRING);
	
	if(send(sockfd, str, strlen(str), ZERO) < ZERO)
		printf("[ERROR]: Socket problem (sending...):");

	return OK;
}


/**
@brief Awaits the response from server with an expected code
@param sockfd - socket file descriptor
@param str - string received (from listen_to)
@param code_str - expected code
@return Returns 0 when succeeds
*/

int receive_from(int sockfd, char *str, char *code_str){


	for(; !test_response(str, code_str);)
		listen_to(sockfd, str);	

	return OK;
}

/**
@brief Checks if the message has the expected code
@param str - string received (message)
@param code_str - expected code
@return Returns ONE (1 -> true) if success (code exists) and 0 (false) otherwise
*/

int test_response(char *str, char *code_str){
	int i, str_pos = ZERO;
	int length = strlen(code_str), position = ZERO;	

	for(; str_pos != ERROR;){
		if(str_pos != ERROR){
			if(str_pos != ZERO) str_pos++;
			
			for(i = ZERO; i < length; i++)
				if(str[str_pos + i] != code_str[i])
					break;

			if(length == i) return ONE;
		}
		str_pos = find_nth(str, NEWLINE, str_pos + ONE, ONE);
	}
	
	return ZERO;
}

/**
@brief Logins the user (anonymous or not) to the server
@param sockfd - socket file descriptor
@param str - string received
@param user - username
@param password - user's password
@return Returns 0 if success
*/

int login(int sockfd, char *str, char *user, char *password){

	receive_from(sockfd, str, CODE_USER);
	send_to(sockfd, str, STR_USER, user);

	receive_from(sockfd, str, CODE_PASSWORD);
	send_to(sockfd, str, STR_PASSWORD, password);

	receive_from(sockfd, str, CODE_USER_LOGGED);

	return OK;

}


/**
@brief If there is a path, sends the CWD (250) code to server and then waits for response
@param sockfd - socket file descriptor
@param str - buffer string
@param url - url path
@return Returns OK (0) if success
*/

int path(int sockfd, char *str, char *url_path){
	
	if (strcmp(url_path, BLANK) != ZERO){
		send_to(sockfd, str, STR_CWD, url_path);
		receive_from(sockfd, str, CODE_CWD);
	}	
	
	return OK;

}

/**
@brief Sends the Passive Mode code to server and then awaits for response
@param sockfd - socket file descriptor
@param str - buffer string
@return Returns OK (0) if success
*/

int passive_mode(int sockfd, char *str){
	
	send_to(sockfd, str, STR_PASV, BLANK);
	receive_from(sockfd, str, CODE_PASV);

	return OK;	

}

/**
@brief Saves the port
@param str - string in the format (%d, %d, %d, %d, %d, %d)
@param port - port
@return Returns OK (0) if success
*/

int port(char *str, int *port){
	
	int pos1, pos2, pos3;
	char *buffer = malloc(MAX_SIZE_WITH_NULL);

	pos1 = find_nth(str, COMMA, ZERO, FOUR);
	pos2 = find_nth(str, COMMA, pos1 + ONE, ONE);
	pos3 = find_nth(str, RIGHT_PARENTHESIS, pos2 + ONE, ONE);	
	
	*port = TWO_POW_EIGHT*atoi(strncpy(buffer, str + pos1 + ONE, pos2)) + atoi(strncpy(buffer, str + pos2 + ONE, pos3));

	return OK;
}

/**
@brief Sends the Size code to server and then awaits for response
@param sockfd - socket file descriptor
@param str - buffer string
@param filename - file name
@param filesize - file size
@return Returns OK (0) if success
*/

int file_size(int sockfd, char *str, char *filename, int *filesize){

	send_to(sockfd, str, STR_SIZE, filename);
	receive_from(sockfd, str, CODE_SIZE);

	*filesize = atoi(&str[FOUR]);
	
	return OK;
}

/**
@brief Sends the Retrieve code to server
@param sockfd - socket file descriptor
@param str - buffer string
@param filename - file name
@return Returns OK (0) if success
*/

int retrieve(int sockfd, char *str, char *filename){
	
	send_to(sockfd, str, STR_RETR, filename);

	return OK;

}

/**
@brief Receives the QUIT command to end connection and sends the response
@param sockfd - socket file descriptor
@param str - string received
@return Returns 0 if success
*/

int quit(int sockfd, char *str){
	
	receive_from(sockfd, str, CODE_QUIT);
	send_to(sockfd, str, STR_QUIT, BLANK);
	
	return OK;

}

/**
@brief Retrieves the file specified
@param sockfd - socket file descriptor
@param filename - file name
@param filesize - file size
@return Returns OK (0) if success
*/

int retrieve_file(int sockfd, char *filename, int filesize){

	FILE *file = fopen(filename, WRITE_BINARY);

	int data_size = FILE_DATA_SIZE;
	char *str = malloc(FILE_DATA_SIZE + ONE);
		
	while(filesize > ZERO){
		if(filesize < FILE_DATA_SIZE) data_size = filesize;

		bzero(str, FILE_DATA_SIZE);

		test_receive(sockfd, str);

		fwrite(str, ONE, data_size, file);

		filesize = filesize - data_size;

		usleep(FILE_SLEEP);
	}

	fclose(file);

	return OK;

}

/**
@brief Initializes the FTP (login, path, passive mode, retrieve and filesize)
@param sockfd - socket file descriptor
@param data - data retrieved from parser
@param retr_port - retrieve port
@param filesize - file size
@return Returns OK (0) if success
*/

int ftp_init(int sockfd, FTP_Data data, int *retr_port, int *filesize){

	char *str = malloc(MAX_SIZE_WITH_NULL);

	login(sockfd, str, data.user, data.password);

	path(sockfd, str, data.url_path);

	passive_mode(sockfd, str);	

	port(str, retr_port);
	
	file_size(sockfd, str, data.filename, filesize);

	return OK;

}

/**
@brief Begins the file transfer
@param sockfd - socket file descriptor
@param ip - internet protocol
@param retr_port - retrieve port
@param filename - file name
@param filesize - file size
@return Returns OK (0) if success
*/

int ftp_transfer(int sockfd, char *ip, int retr_port, char *filename, int filesize){

	int pid = fork();
	char *str = malloc(MAX_SIZE_WITH_NULL);
	
	if(!pid){ 

		retrieve(sockfd, str, filename);		
		sockfd = asocket(ip, retr_port);
		retrieve_file(sockfd, filename, filesize);
		
	} else 
		quit(sockfd, str);

	return OK;

}

/**
@brief Quits the FTP application (closes the socket)
@param sockfd - socket file descriptor
@return Returns OK (0) if success
*/

int ftp_quit(int sockfd){

	usleep(FTP_SLEEP);
	close(sockfd);

	return OK;

}

/**
@brief Begins the FTP application
@param data - data retrieved from parser
*/

int ftp(FTP_Data data){

	char *ip;
	int sockfd;
	int retr_port;
	int filesize;

	ip = getIP(data.host);
	
	sockfd = asocket(ip, data.port);
	
	ftp_init(sockfd, data, &retr_port, &filesize);

	ftp_transfer(sockfd, ip, retr_port, data.filename, filesize);
	
	ftp_quit(sockfd);

}


int main(int argc, char *argv[]) {
	
	test_args(argc, argv);
	
	FTP_Data data;

	parse_data(argv[ONE], &data);

	ftp(data);

	return OK;
}
