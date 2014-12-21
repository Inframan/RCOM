#include "ftp.h"

int login(int sockfd, char *str, char *user, char *password){

	receive_from(sockfd, str, CODE_USER);
	send_to(sockfd, str, STR_USER, user);

	receive_from(sockfd, str, CODE_PASSWORD);
	send_to(sockfd, str, STR_PASSWORD, password);

	receive_from(sockfd, str, CODE_USER_LOGGED);

	return OK;

}

int path(int sockfd, char *str, char *url_path){
	
	if (strcmp(url_path, BLANK) != ZERO){
		send_to(sockfd, str, STR_CWD, url_path);
		receive_from(sockfd, str, CODE_CWD);
	}	
	
	return OK;

}

int passive_mode(int sockfd, char *str){
	
	send_to(sockfd, str, STR_PASV, BLANK);
	receive_from(sockfd, str, CODE_PASV);

	return OK;	

}

int port(char *str, int *port){
	
	int pos1, pos2, pos3;
	char *buffer = malloc(MAX_SIZE_WITH_NULL);

	pos1 = find_nth(str, COMMA, ZERO, FOUR);
	pos2 = find_nth(str, COMMA, pos1 + ONE, ONE);
	pos3 = find_nth(str, RIGHT_PARENTHESIS, pos2 + ONE, ONE);	
	
	*port = TWO_POW_EIGHT*atoi(strncpy(buffer, str + pos1 + ONE, pos2)) + atoi(strncpy(buffer, str + pos2 + ONE, pos3));

	return OK;
}

int file_size(int sockfd, char *str, char *filename, int *filesize){

	send_to(sockfd, str, STR_SIZE, filename);
	receive_from(sockfd, str, CODE_SIZE);

	*filesize = atoi(&str[FOUR]);
	
	return OK;
}

int retrieve(int sockfd, char *str, char *filename){
	
	send_to(sockfd, str, STR_RETR, filename);

	return OK;

}

int quit(int sockfd, char *str){
	
	receive_from(sockfd, str, CODE_QUIT);
	send_to(sockfd, str, STR_QUIT, BLANK);
	
	return OK;

}

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

int ftp_valid(char *message){

	if(strstr(message, FTP_CODE_NO_FILE) != NULL)
		exit(ABORT);
	else if(strstr(message, FTP_CODE_WRONG_CREDENTIALS) != NULL)
		exit(ABORT);
	else
		return OK;

}

int ftp_init(int sockfd, FTP_Data data, int *retr_port, int *filesize){

	char *str = malloc(MAX_SIZE_WITH_NULL);

	login(sockfd, str, data.user, data.password);

	path(sockfd, str, data.url_path);

	passive_mode(sockfd, str);	

	port(str, retr_port);
	
	file_size(sockfd, str, data.filename, filesize);

	return OK;

}

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

int ftp_quit(int sockfd){

	usleep(FTP_SLEEP);
	close(sockfd);

	return OK;

}

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
