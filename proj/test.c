#include "test.h"

int test_receive(int sockfd, char *str){

	if(recv(sockfd, str, MAX_SIZE, ZERO) < ZERO){
		printf("[ERROR]: %s", strerror(errno));
		return !OK;
	}

	return OK;

}

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
