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
@brief Checks if the only argument that exists is the FTP request
@param argc - number of parameters
@return Returns 0 if success
*/

int checkArgc(int argc){
	if (argc != 2) {
		printf("Program: ./download ftp://[<user>:<password>@]<host>[:port]/<url-path>\n");
		exit(-1);
	}
	return 0;
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
		exit(-1);
	}

	printf("Hostname: %s\n", h->h_name);

	char *ip = inet_ntoa(*((struct in_addr *)h->h_addr));

	printf("IP Address : %s\n", inet_ntoa(*((struct in_addr *)h->h_addr)));
	
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

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("[ERROR]: Socket could not be created");
		exit(-1);
	}
	
	if(inet_aton(ip, &address) == 0) {
		printf("[ERROR]: Failure getting an IP");
		exit(-1);
	}

	server_addr.sin_addr.s_addr = address.s_addr;
	
	//Connect to the server.
	if(connect(sockfd, (struct sockaddr*) &server_addr, sizeof (server_addr)) != 0){
		printf("[ERROR]: Failure connecting to hport");
		exit(-1);
	}

	return sockfd;
}

/**
@brief Listens to any message that comes from server
@param sockfd - socket file descriptor
@param str - string to get read message
@return Returns 0 if success
*/

int listenTo(int sockfd, char *str){

	bzero(str, MAX_SIZE);

	if(recv(sockfd, str, MAX_SIZE, 0) < 0)
		printf("[ERROR]: Socket problem (receiving...) ");

	printf("String listened: %s\n", str);

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

int sendTo(int sockfd, char *str, char *code_str, char *value){

	bzero(str, MAX_SIZE);

	strcpy(str, code_str);
	strcat(str, value);
	strcat(str, "\n");
	
	if(send(sockfd, str, strlen(str), 0) < 0)
		printf("[ERROR]: Socket problem (sending...): ");

	return OK;
}

/**
@brief Checks if the message has the expected code
@param str - string received (message)
@param code_str - expected code
@return Returns 1 if success (code exists) and 0 otherwise
*/

int testResponse(char *str, char *code_str){
	int i, str_pos = 0;
	int length = strlen( code_str ), position = 0;	

	for(; str_pos != -1;){

		if(str_pos != -1){
			if(str_pos != 0) str_pos++;
			
			for(i = 0; i < length; i++)
				if(str[str_pos + i] != code_str[i])
					break;

			if(length == i) return 1;
		}
		str_pos = find(str, '\n', str_pos + 1);
	}
	
	return 0;
}

/**
@brief Awaits the response from server with an expected code
@param sockfd - socket file descriptor
@param str - string received (from listenTo)
@param code_str - expected code
@return Returns 0 when succeeds
*/

int receiveFrom(int sockfd, char *str, char *code_str){

	for(; !testResponse(str, code_str);)
		listenTo(sockfd, str);	

	return OK;
}

/**
@brief Logins the user (anonymous or not) to the server
@param sockfd - socket file descriptor
@param str - string received
@param data - data retrieved from parser
@return Returns 0 if success
*/

int login(int sockfd, char *str, FTP_Data data){

	receiveFrom( sockfd, str, CODE_USER );
	sendTo( sockfd, str, STR_USER, data.user );

	receiveFrom( sockfd, str, CODE_PASSWORD );
	sendTo( sockfd, str, STR_PASSWORD, data.password );

	receiveFrom( sockfd, str, CODE_USER_LOGGED );

	return OK;

}


/**
@brief Receives the QUIT command to end connection and sends the response
@param sockfd - socket file descriptor
@param str - string received
@return Returns 0 if success
*/

int quit(int sockfd, char *str){
	
	receiveFrom( sockfd, str, CODE_QUIT );
	sendTo( sockfd, str, STR_QUIT, BLANK );
	
	printf("QUIT\n");	

	return OK;

}


/*************************************************************************************

				NEEDS TO BE CHANGED


*************************************************************************************/



FTP_Data createURL(char* input) {
	FTP_Data result;
	if (input == NULL) {
		printf("[ERROR]: Nothing was indicated");
		exit(-1);
	}

	if (strncmp(input, "ftp://", 6) == 0) {
		
	} else {
		printf("[ERROR]: It should start by ftp://" );
		exit(-1);
	}

	result.user = "anonymous";
	result.password = "anonymous@anonymous.com";
	result.port = 21;

	int colon_pos, at_pos = -1, slash_pos, final_slash_pos, tmp_final;
	colon_pos = find( input, ':', 6 );
	slash_pos = find( input, '/', 6 );

	if( slash_pos == -1 ){
		printf("[ERROR]: Check the syntax from FTP again");
		exit(-1);
	}

	if( colon_pos != -1 ){ 
		at_pos = find( input, '@', colon_pos+1 );
		if( at_pos != -1 ){ 
			result.user = str_copy( input, 6, colon_pos );
			result.password = str_copy( input, colon_pos+1, at_pos );
			colon_pos = find( input, ':', at_pos+1 ); 
		}
		if( colon_pos != -1 ){ 
			char *port_str = str_copy( input, colon_pos+1, slash_pos );
			result.port = atoi( port_str );
		}
	}
	
	
	if( at_pos == -1 )
		at_pos = 5;
	if( colon_pos == -1 )
		colon_pos = slash_pos;
	result.host = str_copy( input, at_pos+1, colon_pos );
	
	
	final_slash_pos = slash_pos+1;
	tmp_final = find( input, '/', final_slash_pos );
	while( tmp_final != -1 ){
		final_slash_pos = tmp_final;
		tmp_final = find( input, '/', tmp_final+1 );
	};
	result.url_path = str_copy( input, slash_pos+1, final_slash_pos );
	if( final_slash_pos == slash_pos+1 )
		final_slash_pos--;
	result.filename = str_copy( input, final_slash_pos+1, strlen(input) );



	return result;
}


int get_port( char *buffer ){
	
	int port,
		port_pos1,
		port_pos2,
		port_pos3;
	char *port_str1,
		 *port_str2;

	port_pos1 = find_nth( buffer, ',', 0, 4 );
	port_pos2 = find( buffer, ',', port_pos1+1 );
	port_pos3 = find( buffer, ')', port_pos2+1 );
	
	port_str1 = str_copy( buffer, port_pos1+1, port_pos2 );
	port_str2 = str_copy( buffer, port_pos2+1, port_pos3 );
	
	port = 256*atoi( port_str1 ) + atoi( port_str2 );

	return port;
}


int main(int argc, char *argv[]) {
	
	printf("-------------FTP INIT--------------\n");

	checkArgc(argc);
	
	FTP_Data url = createURL( argv[1] );

	char *str = malloc(MAX_SIZE + 1);
	char *ip = getIP( url.host );
	int sockfd = asocket( ip, url.port );

	login(sockfd, str, url);

	if ( strcmp( url.url_path, BLANK ) != 0 ){
		sendTo( sockfd, str, STR_CWD, url.url_path );
		receiveFrom( sockfd, str, CODE_CWD );
	}

	sendTo( sockfd, str, STR_PASV, BLANK );
	receiveFrom( sockfd, str, CODE_PASV );
	int retr_port = get_port( str );

	sendTo( sockfd, str, STR_SIZE, url.filename );
	receiveFrom( sockfd, str, CODE_SIZE );
	unsigned int fileSize = atoi( &str[4] );
	sendTo( sockfd, str, STR_RETR, url.filename );

	usleep( DEFAULT_USLEEP );

	if( !fork() ){ 
		sockfd = asocket( ip, retr_port );
		
		FILE *file = fopen( url.filename, "wb" );
		unsigned char* fbuffer[ MAX_SIZE+1 ];
		fbuffer[ MAX_SIZE ] = '\0';
		int current_chunk = MAX_SIZE;
		
		while( fileSize > 0 ){

			if( fileSize < MAX_SIZE )
				current_chunk = fileSize;

			bzero( fbuffer, MAX_SIZE );
			if( recv( sockfd, fbuffer, MAX_SIZE, 0 ) < 0 ){
				printf("Error while receiving socket:");
			}

			fwrite( fbuffer, sizeof(char), current_chunk, file );
			fileSize -= current_chunk;
			usleep( FBUFFER_USLEEP );
		}

		fclose( file );
	} else 
		quit(sockfd, str);

	close(sockfd);

	return 0;
}
