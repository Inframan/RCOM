#include <stdio.h>
#include "defines.h"
#include "ftp_data.h"

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
		printf("[ERROR]: Argument is missing\n");
		exit(ABORT);
	} else if(strncmp(argv[ONE], FTP_START, SIX) != ZERO){
		printf("[ERROR]: It should start by ftp://\n");
		exit(ABORT);
	}

	return OK;
}

int main(int argc, char *argv[]) {
	
	test_args(argc, argv);
	
	FTP_Data data;

	parse_data(argv[ONE], &data);

	ftp(data);

	return OK;
}
