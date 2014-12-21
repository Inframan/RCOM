
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

void init(FTP_Data *data){

	data->user = malloc(MAX_SIZE_WITH_NULL);
	data->password = malloc(MAX_SIZE_WITH_NULL);
	data->host = malloc(MAX_SIZE_WITH_NULL);
	data->url_path = malloc(MAX_SIZE_WITH_NULL);
	data->filename = malloc(MAX_SIZE_WITH_NULL);

}	

/**
@brief Sets the default settings (user, password and port)
@param data - data
*/

void set_default(FTP_Data *data){

	strcpy(data->user, USER_ANONYMOUS);
	strcpy(data->password, PASSWORD_ANONYMOUS);
	data->port = FTP_PORT;

}

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

void set_all(FTP_Data *data, char *user, char *password, char *host, char *url_path, char *filename){

	strcpy(data->user, user);
	strcpy(data->password, password);
	strcpy(data->host, host);
	strcpy(data->url_path, url_path);
	strcpy(data->filename, filename);

}

/**
@brief Parses the data from arguments received
@param arg - argv[1]
@param data - data
@return Returns OK (0) if success
*/

int parse_data(char *arg, FTP_Data *data) {

	int tmp_final;
	int colon_pos, at_pos = ERROR, slash_pos, final_slash_pos;	

	init(data);
	set_default(data);

	colon_pos = find_nth(arg, COLON, SIX, ONE);
	slash_pos = find_nth(arg, SLASH, SIX, ONE);

	if(colon_pos != NOT_FOUND){ 
		at_pos = find_nth(arg, AT, colon_pos + ONE, ONE);

		if(at_pos != NOT_FOUND){ 
			data->user = str_copy(arg, SIX, colon_pos);
			data->password = str_copy(arg, colon_pos + ONE, at_pos);
			colon_pos = find_nth(arg, COLON, at_pos + ONE, ONE); 
		}

		if(colon_pos != NOT_FOUND)
			data->port = atoi(str_copy(arg, colon_pos + ONE, slash_pos));

	}
	
	
	if(at_pos == NOT_FOUND) at_pos = FIVE;

	if(colon_pos == NOT_FOUND) colon_pos = slash_pos;

	final_slash_pos = slash_pos + ONE;
	tmp_final = find_nth(arg, SLASH, final_slash_pos, ONE);

	while(tmp_final != NOT_FOUND){
		final_slash_pos = tmp_final;
		tmp_final = find_nth(arg, SLASH, tmp_final + ONE, ONE);
	}
	
	data->url_path = str_copy(arg, slash_pos + ONE, final_slash_pos);
	data->host = str_copy(arg, at_pos + ONE, colon_pos);

	if(final_slash_pos == slash_pos + ONE) final_slash_pos--;	

	data->filename = str_copy(arg, final_slash_pos + ONE, strlen(arg));

	return OK;
}
