#include "config.h"

char* getIP(char *hostname){

	struct hostent *h;

	if((h = gethostbyname(hostname)) == NULL){
		herror("gethostbyname");
		exit(ABORT);
	}

	printf("Hostname: %s\n", h->h_name);

	char *ip = inet_ntoa(*((struct in_addr *)h->h_addr));

	printf("\nIP Address : %s\n\n", inet_ntoa(*((struct in_addr *)h->h_addr)));
	
	return ip;
}

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
