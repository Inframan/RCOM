#include "message.h"

int listen_to(int sockfd, char *str){

	bzero(str, MAX_SIZE);
	
	test_receive(sockfd, str);

	printf("%s\n", str);

	ftp_valid(str);

	return OK;
}

int send_to(int sockfd, char *str, char *code_str, char *value){

	bzero(str, MAX_SIZE);

	strcpy(str, code_str);
	strcat(str, value);
	strcat(str, NEWLINE_STRING);
	
	if(send(sockfd, str, strlen(str), ZERO) < ZERO)
		printf("[ERROR]: Socket problem (sending...):");

	return OK;
}

int receive_from(int sockfd, char *str, char *code_str){

	for(; !test_response(str, code_str);)
		listen_to(sockfd, str);	

	return OK;
}
