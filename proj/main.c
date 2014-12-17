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

	printf( "%s", str );
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




//Converts the user input into a well-defined and separated struct after parsing through it.
FTP_Data createURL(char* input) {
	FTP_Data result;
	if (input == NULL) {
		printf("[ERROR]: Nothing was indicated");
		exit(-1);
	}

	// RFC1738 : ftp://[<user>:<password>@]<host>[:port]/<url-path>

	// Check if it starts with ftp://
	if (strncmp(input, "ftp://", 6) == 0) {
		printf( "Found \"ftp://\", continuing.\n" );
	} else {
		printf("[ERROR]: It should start by ftp://" );
		exit(-1);
	}

	// Default user and password, in case it's not found in the parsed input.
	result.user = "anonymous";
	result.password = "anonymous@anonymous.com";

	// Default port.
	result.port = 21;

	// Check for user:password@ and :port
	int colon_pos, at_pos = -1, slash_pos, final_slash_pos, tmp_final;
	colon_pos = find( input, ':', 6 );
	slash_pos = find( input, '/', 6 );

	//There needs to be at least one slash, so we check for it immediately and abort if there isn't one.
	if( slash_pos == -1 ){
		printf("[ERROR]: Check the syntax from FTP again");
		exit(-1);
	}

	if( colon_pos != -1 ){ //At least 1 colon was found.
		at_pos = find( input, '@', colon_pos+1 );
		if( at_pos != -1 ){ //If we found the @ symbol, then we found the username and password.
			result.user = str_copy( input, 6, colon_pos );
			result.password = str_copy( input, colon_pos+1, at_pos );
			colon_pos = find( input, ':', at_pos+1 ); //Try to find the 'port', if it's specified.
		}
		if( colon_pos != -1 ){ //At this point, the colon_pos should always be relevant to the port.
			char *port_str = str_copy( input, colon_pos+1, slash_pos );
			result.port = atoi( port_str );
		}
	}
	
	//Get the host.
	if( at_pos == -1 )
		at_pos = 5;
	if( colon_pos == -1 )
		colon_pos = slash_pos;
	result.host = str_copy( input, at_pos+1, colon_pos );
	
	//Get the URL and file.
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

/*
	//Print the parsed information.
	printf( "Username: %s\nPassword: %s\n--------\n\n", result.user, result.password );
	printf( "Hostname: %s\nPortword: %d\n--------\n\n", result.host, result.port );
	printf( "Pathname: %s\nFileword: %s\n--------\n\n", result.url_path, result.filename );
*/

	return result;
}

//Parse the line received from PASV to get the port that we should connect to next.
int get_port( char *buffer ){
	//227 Entering Passive Mode (192,168,50,143,229,63).
	//227 Entering Passive Mode (192,168,50,143,201,243).
	//Etc.
	int port,
		port_pos1,
		port_pos2,
		port_pos3;
	char *port_str1,
		 *port_str2;
	//Get to the ports in the string.
	port_pos1 = find_nth( buffer, ',', 0, 4 );
	port_pos2 = find( buffer, ',', port_pos1+1 );
	port_pos3 = find( buffer, ')', port_pos2+1 );
	//Copy the relevant parts and interpret them.
	port_str1 = str_copy( buffer, port_pos1+1, port_pos2 );
	port_str2 = str_copy( buffer, port_pos2+1, port_pos3 );
	//Add the two bytes to the (retr) port.
	port = 256*atoi( port_str1 ) + atoi( port_str2 );
/*
	//Print the two retrieved bytes as well as the interpreted port:
	printf( "\nPortByte 1: %s\nPortByte 2: %s\nPort: %d\n----\n\n", port_str1, port_str2, port );
*/
	//Return the interpreted port.
	return port;
}


//Sends a message and waits to receive one in return.
void full_message( int sockfd, char* buffer, char *str1, char *str2 ){
	//Send message.
	sendTo( sockfd, buffer, str1, str2 );

	//Receive response.
	listenTo( sockfd, buffer );
	
	//Wait a little bit.
	usleep( DEFAULT_USLEEP );
}

//Checks if str has any line starting with code_str. Returns 1 on success, 0 on failure.
int has_code( char *str, char *code_str ){
	int len = strlen( code_str ),
		i,
		pos = 0;
	//We loop through the string finding every newline character, until there are no more.
	do{
		//If we did find a newline character, we process it.
		if( pos != -1 ){
			//Special condition to ensure we also check the very start of str.
			if( pos != 0 ){
				pos++;
			}
			//Loop through the start of the line to check if it is the code that we want.
			for( i=0; i<len; i++ ){
				if( str[ pos+i ] != code_str[i] ){
					break;
				}
			}
			//If all the characters were equal, then i=len and we can return success.
			if( i==len ){
				return 1;
			}
			//Otherwise, we haven't found anything yet and we let the loop continue.
		}
		pos = find( str, '\n', pos+1 );
	} while( pos != -1 );
	//If we have found no more newline characters, then at this point the code isn't in the string, so we just return failure.
	return 0;
}

//Waits for the buffer to have a line starting with 'code_str'.
void wait_for_code( int sockfd, char *buffer, char *code_str ){
	do{
		listenTo( sockfd, buffer );
	} while( !has_code( buffer, code_str ) );
}


int main(int argc, char** argv) {
	
	checkArgc(argc);

	// Parse argv[1] into a struct.
	FTP_Data url = createURL( argv[1] );

	char buffer[ MAX_SIZE+1 ];
	char * destIp = getIP( url.host );
	int sockfd = asocket( destIp, url.port );

	// Ensure that the buffer is ALWAYS null-terminated, hence why it has MAX_SIZE+1 as its size instead of just MAX_SIZE.
	buffer[MAX_SIZE] = '\0';

	// Get welcome message, which should start with "220" in its latest line or at least one of the lines.
	wait_for_code( sockfd, buffer, CODE_USER );

	// Send the parsed username.
	sendTo( sockfd, buffer, "USER ", url.user );

	// Password, code 331.
	wait_for_code( sockfd, buffer, CODE_PASSWORD );
	sendTo( sockfd, buffer, "PASS ", url.password );

	// CWD debian/doc, for example, or the directory specified by the user, after code 230.
	wait_for_code( sockfd, buffer, CODE_USER_LOGGED );
	if ( strcmp( url.url_path, "" ) != 0 ){
		sendTo( sockfd, buffer, "CWD ", url.url_path );
		//If we did do CWD, we then wait for code 250.
		wait_for_code( sockfd, buffer, CODE_CWD );
	}

	//PASV, where codes 230 and 250 have been parsed before already, if necessary.
	sendTo( sockfd, buffer, "PASV", "" );
	
	//Wait for code 227, with the format: 227 Entering Passive Mode (192,168,50,143,201,243).
	wait_for_code( sockfd, buffer, CODE_PASV );
	int retr_port = get_port( buffer );

	//SIZE has already had the previous message wait for it.
	sendTo( sockfd, buffer, "SIZE ", url.filename );
	wait_for_code( sockfd, buffer, CODE_SIZE );
	unsigned int fileSize = atoi( &buffer[4] );

	//Send RETR message.
	sendTo( sockfd, buffer, "RETR ", url.filename );

	//Wait a little bit.
	usleep( DEFAULT_USLEEP );

	//Get file.
	if( !fork() ){ //int parent = fork();
		//We are the chosen one.
		//I mean, the child.
		sockfd = asocket( destIp, retr_port );
		
		FILE *file = fopen( url.filename, "wb" );
		unsigned char* fbuffer[ MAX_SIZE+1 ];
		fbuffer[ MAX_SIZE ] = '\0';
		int current_chunk = MAX_SIZE;
		
		while( fileSize > 0 ){
			//Ensure that the current chunk we'll be writing to the file always has the correct size.
			if( fileSize < MAX_SIZE )
				current_chunk = fileSize;

			//Zero the buffer.
			bzero( fbuffer, MAX_SIZE );
			//Receive response.
			if( recv( sockfd, fbuffer, MAX_SIZE, 0 ) < 0 ){
				perror("Error while receiving socket:");
			}

			fwrite( fbuffer, sizeof(char), current_chunk, file );
			fileSize -= current_chunk;

			//Wait a little bit.
			usleep( FBUFFER_USLEEP );
		}
		//Close the file descriptor.
		fclose( file );
	} else {
		//Wait for "226" code.
		wait_for_code( sockfd, buffer, CODE_QUIT );
		//Send QUIT message.
		sendTo( sockfd, buffer, "QUIT", "" );
		printf("Disconnecting from remote host...\n");
	}

	close( sockfd );
	return 0;
}
