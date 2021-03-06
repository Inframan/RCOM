
/**
@brief Struct to save important data related to ftp.
@var user - username
@var password - password
@host - hostname
@url_path - url path
*/
typedef struct{
	char *user = NULL;
	char *password = NULL;
	char *host;
	char *url_path;
} FTP_Data;