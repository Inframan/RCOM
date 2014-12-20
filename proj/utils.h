int find_nth(char* str, char c, int start_pos, int nth){

	if(start_pos >= strlen(str)) return -1;

	for(; str[start_pos]!='\0'; start_pos++){
		if(str[start_pos] == c){
			if(nth == 1)
				return start_pos;
			else
				nth--;
		}
	}
	return -1;
}

char *str_copy(char* str, int start, int end){

	int length = end - start;
	char *ret = malloc( length + 1 );

	memcpy(ret, &str[start], length);
	ret[length] = '\0';

	return ret;
}
