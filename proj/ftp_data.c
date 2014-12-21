#include "ftp_data.h"

void init(FTP_Data *data){

	data->user = malloc(MAX_SIZE_WITH_NULL);
	data->password = malloc(MAX_SIZE_WITH_NULL);
	data->host = malloc(MAX_SIZE_WITH_NULL);
	data->url_path = malloc(MAX_SIZE_WITH_NULL);
	data->filename = malloc(MAX_SIZE_WITH_NULL);

}

void set_default(FTP_Data *data){

	strcpy(data->user, USER_ANONYMOUS);
	strcpy(data->password, PASSWORD_ANONYMOUS);
	data->port = FTP_PORT;

}

void set_all(FTP_Data *data, char *user, char *password, char *host, char *url_path, char *filename){

	strcpy(data->user, user);
	strcpy(data->password, password);
	strcpy(data->host, host);
	strcpy(data->url_path, url_path);
	strcpy(data->filename, filename);

}

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
			data->user = str_cpy(arg, SIX, colon_pos);
			data->password = str_cpy(arg, colon_pos + ONE, at_pos);
			colon_pos = find_nth(arg, COLON, at_pos + ONE, ONE); 
		}

		if(colon_pos != NOT_FOUND)
			data->port = atoi(str_cpy(arg, colon_pos + ONE, slash_pos));

	}
	
	
	if(at_pos == NOT_FOUND) at_pos = FIVE;

	if(colon_pos == NOT_FOUND) colon_pos = slash_pos;

	final_slash_pos = slash_pos + ONE;
	tmp_final = find_nth(arg, SLASH, final_slash_pos, ONE);

	while(tmp_final != NOT_FOUND){
		final_slash_pos = tmp_final;
		tmp_final = find_nth(arg, SLASH, tmp_final + ONE, ONE);
	}
	
	data->url_path = str_cpy(arg, slash_pos + ONE, final_slash_pos);
	data->host = str_cpy(arg, at_pos + ONE, colon_pos);

	if(final_slash_pos == slash_pos + ONE) final_slash_pos--;	

	data->filename = str_cpy(arg, final_slash_pos + ONE, strlen(arg));

	return OK;
}
