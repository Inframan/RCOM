#ifndef _FTP_DATA_
#define _FTP_DATA_

#include "ftp_data.h"

/**
@brief Saves the data after parsing the arguments
@ftp_data - struct ftp_data
@user - username
@password - password
@host - hostname
@url_path - url path
*/

void saveData((struct FTP_Data *) ftp_data, char *user, char *password, char *host, char *url_path){

	ftp_data->user = user;
	ftp_data->password = password;
	ftp_data->host = host;
	ftp_data->url_path = url_path;

}

#endif