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

char* getIP(char* hostname) {
	struct hostent *h;

	if ( ( h = gethostbyname(hostname) ) == NULL ) {
		herror("gethostbyname");
		exit(-1);
	}

	printf("Hostname: %s\n", h->h_name);

	char *ip = inet_ntoa(*((struct in_addr *)h->h_addr));

	printf("IP Address : %s\n", inet_ntoa(*((struct in_addr *)h->h_addr)));
	
	return ip;
}

/**	
@brief Tries to assign the socket file descriptor	
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

	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
		printf("[ERROR]: Socket could not be created");
		exit(-1);
	}
	
	if( inet_aton(ip, &address) == 0 ) {
		printf( "[ERROR]: Failure getting an IP" );
		exit(-1);
	}

	server_addr.sin_addr.s_addr = address.s_addr;
	
	//Connect to the server.
	if( connect( sockfd, (struct sockaddr*) &server_addr, sizeof (server_addr) ) != 0 ){
		printf("[ERROR]: Failure connecting to hport");
		exit(-1);
	}

	return sockfd;
}

/**
@brief Listens to any message that comes from server
@param sockfd - socket file descriptor
@param str - string to get read message
*/

void listenTo( int sockfd, char *str ){

	bzero( str, MAX_SIZE );

	if( recv( sockfd, str, MAX_SIZE, 0 ) < 0 )
		perror( "[ERROR]: Socket problem (receiving...) " );

	printf( "LISTENING_TO_STRING ||||| %s", str );
}

/**
@brief Sends info to server (USER | PASS | etc...)
@param sockfd - socket file descriptor
@param str - string to be sent
@param code_str - USER | PASS | PASV | etc...
@param value - value
@return Returns 0 if success
*/

int sendTo( int sockfd, char *str, char *code_str, char *value ){

	bzero( str, MAX_SIZE );

	strcpy(str, code_str);
	strcat(str, value);
	strcat(str, "\n");
	
	if(send(sockfd, str, strlen(str), 0) < 0)
		perror("[ERROR]: Socket problem (sending...): ");

	return 0;
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



void full_message( int sockfd, char* buffer, char *str1, char *str2 ){
	sendTo( sockfd, buffer, str1, str2 );
	listenTo( sockfd, buffer );
	usleep( DEFAULT_USLEEP );
}

int has_code( char *str, char *code_str ){
	int len = strlen( code_str ),
		i,
		pos = 0;
	
	do{
		
		if( pos != -1 ){
			
			if( pos != 0 ){
				pos++;
			}
			
			for( i=0; i<len; i++ ){
				if( str[ pos+i ] != code_str[i] ){
					break;
				}
			}
			
			if( i==len ){
				return 1;
			}
			
		}
		pos = find( str, '\n', pos+1 );
	} while( pos != -1 );
	
	return 0;
}


void wait_for_code( int sockfd, char *buffer, char *code_str ){
	do{
		listenTo( sockfd, buffer );
	} while( !has_code( buffer, code_str ) );
}


int main(int argc, char** argv) {
	
	printf("-------------FTP INIT--------------\n");

	checkArgc(argc);

	
	FTP_Data url = createURL( argv[1] );

	char buffer[ MAX_SIZE+1 ];
	char * destIp = getIP( url.host );
	int sockfd = asocket( destIp, url.port );

	buffer[MAX_SIZE] = '\0';
	wait_for_code( sockfd, buffer, CODE_USER );

	sendTo( sockfd, buffer, "USER ", url.user );

	wait_for_code( sockfd, buffer, CODE_PASSWORD );
	sendTo( sockfd, buffer, "PASS ", url.password );

	wait_for_code( sockfd, buffer, CODE_USER_LOGGED );
	if ( strcmp( url.url_path, "" ) != 0 ){
		sendTo( sockfd, buffer, "CWD ", url.url_path );
		wait_for_code( sockfd, buffer, CODE_CWD );
	}

	sendTo( sockfd, buffer, "PASV", "" );
	wait_for_code( sockfd, buffer, CODE_PASV );
	int retr_port = get_port( buffer );

	sendTo( sockfd, buffer, "SIZE ", url.filename );
	wait_for_code( sockfd, buffer, CODE_SIZE );
	unsigned int fileSize = atoi( &buffer[4] );
	sendTo( sockfd, buffer, "RETR ", url.filename );

	usleep( DEFAULT_USLEEP );

	if( !fork() ){ 
		sockfd = asocket( destIp, retr_port );
		
		FILE *file = fopen( url.filename, "wb" );
		unsigned char* fbuffer[ MAX_SIZE+1 ];
		fbuffer[ MAX_SIZE ] = '\0';
		int current_chunk = MAX_SIZE;
		
		while( fileSize > 0 ){

			if( fileSize < MAX_SIZE )
				current_chunk = fileSize;

			bzero( fbuffer, MAX_SIZE );
			if( recv( sockfd, fbuffer, MAX_SIZE, 0 ) < 0 ){
				perror("Error while receiving socket:");
			}

			fwrite( fbuffer, sizeof(char), current_chunk, file );
			fileSize -= current_chunk;
			usleep( FBUFFER_USLEEP );
		}

		fclose( file );
	} else {

		wait_for_code( sockfd, buffer, CODE_QUIT );
		sendTo( sockfd, buffer, "QUIT", "" );
		printf("-----------------EXITING FTP------------------\n");
	}

	close( sockfd );
	return 0;
}
