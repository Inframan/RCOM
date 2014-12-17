
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
Some methods maybe we'll add here...
*/
