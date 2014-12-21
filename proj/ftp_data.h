#ifndef _FTP_DATA_H_
#define _FTP_DATA_H_

#include "defines.h"

/**
Struct to save all info related to FTP
Includes saving:
User and Password (OPTIONAL)
Host and UrlPath and Filename (NOT OPTIONAL)
Port (OPTIONAL)
*/

typedef struct {
	char *user, *password;
	char *host, *url_path, *filename;
	int port;
} FTP_Data;

/**
@brief Allocates memory for variables
@param data - data
*/

void init(FTP_Data *data);	

/**
@brief Sets the default settings (user, password and port)
@param data - data
*/

void set_default(FTP_Data *data);

/**
@brief Sets all data
@param data - data
@param user - username
@param password - user's password
@param host - hostname
@param url_path - url path
@param filename - file name
@param port - port
*/

void set_all(FTP_Data *data, char *user, char *password, char *host, char *url_path, char *filename);

/**
@brief Parses the data from arguments received
@param arg - argv[1]
@param data - data
@return Returns OK (0) if success
*/

int parse_data(char *arg, FTP_Data *data);

#endif
