#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <errno.h> 
#include <netdb.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>

#include "defines.h"
#include "ftp_data.h"


int parseArgs(char *argv[], (struct FTP_Data *) ftp_data){

	unsigned int i = 4;

	char *ftp;
	char *user = NULL;
	char *password = NULL;
	char *host = (char *) malloc(MAX_SIZE);
	char *url_path = (char *) malloc(MAX_SIZE);

	char *colon;
	char *at;
	char *slash;

	memcpy(ftp, argv, 6);

	if( strcmp(ftp, "ftp://") != 0 ) //if there is no part of ftp
		exit(-1);

	if( slash = strchr(argv, "/") == NULL) //if there is no slash separating the host and url path
		exit(-1);

	if( colon = strchr(argv, ":") == NULL ){ //if there is no colon

		if( at = strchr(argv, "@") == NULL ) //it shouldn't have at (arroba)
			exit(-1);

	} else{

		if( at = strchr(argv, "@") == NULL ) //if there is no at (when there is a colon), it should end the program
			exit(-1);

		user = (char *) malloc(MAX_SIZE);
		password = (char *) malloc(MAX_SIZE);

		//Get User

		for(i++; argv[i] != colon; i++){
			strcat(user, argv[i]);
		}

		//Get Password

		for(i++; argv[i] != at; i++){
			strcat(password, argv[i]);
		}

	}

	//Get Host

	for(i++; argv[i] != slash; i++)
		strcat(host, argv[i]);

	//Get Url-Path

	for(i++; argv[i] != NULL; i++)
		strcat(url_path, argv[i]);

	return 0;
}


/**
@brief Parses the program arguments (displays the usage if they're not right)
@param argv - arguments
@return 0 if success, exits the program if error (-
 == 0*/

int parseArgv(int argc, char *argv[], (struct FTP_Data *) ftp_data){

	if( argc != 2){
		printf("ftp://[<user>:<password>@]<host>/<url-path>\n");
		exit(-1);
	}

	parseArgs(argv, ftp_data);

	return 0;
}

/**
@brief Establishes the connection based on hostname
@param h - struct hostent
@param hostname - hostname
*/

void getIP((struct hostent *) h, char *hostname){

	if ( ( h = gethostbyname(hostname) == NULL ) ) {  
		herror("gethostbyname");
		exit(1);
	}

	printf("Host name  : %s\n", h->h_name);
	printf("IP Address : %s\n",inet_ntoa(*((struct in_addr *)h->h_addr)));

}

/**
@brief Tries to assign the socket file descriptor
@param sockfd - socket file descriptor
@return 0 if success, exits the program if error (-1)
*/

int asocket(int *sockfd, char *address, int port){

	struct sockaddr_in server_addr;
	
	/*server address handling*/
	bzero( (char*) &server_addr, sizeof(server_addr) );
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(adress);	/*32 bit Internet address network byte ordered*/
	server_addr.sin_port = htons(port);		/*server TCP port must be network byte ordered */
    
	/*open an TCP socket*/
	if ( (*sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0 ) {
		perror("socket()");
		exit(-1);
	}
	/*connect to the server*/
    if( connect(*sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0 ){
		perror("connect()");
		exit(-1);
	}

	return 0;

}

int main(int argc, char *argv[]){

	int sockfd;
	struct hostent *h;
	FTP_Data ftp_data;


	parseArgv(argc, argv, &ftp_data);

	getIP(h, argv[1]);


	asocket(&sockfd);

	close(sockfd);

	return 0;
}