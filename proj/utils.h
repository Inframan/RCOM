//Returns the nth (starting at 1) occurrence of 'c' in 'str' from the starting position. Returns -1 on failure.
int find_nth( char* str, char c, int start_pos, int nth ){
	//Abort immediately if we're trying to start beyond the string.
	if( start_pos >= strlen(str) ){
		return -1;
	}
	//Try to find the nth character in the string.
	for( ; str[start_pos]!='\0'; start_pos++ ){
		if( str[start_pos] == c ){
			if( nth == 1 ){
				return start_pos;
			} else {
				nth--;
			}
		}
	}
	//If we've reached the end of the string, return failure.
	return -1;
}

//Returns the first occurrence of 'c' in 'str' from the starting position. Returns -1 on failure.
int find( char* str, char c, int start_pos ){
	//Simplify the code by calling just this instead?
	return find_nth( str, c, start_pos, 1 );
}

//Returns a pointer to a null-ending string copied from str at start to end.
char *str_copy( char* str, int start, int end ){
	//Get the length of the part of the string we're going to copy.
	int length = end-start;
	//Allocate memory.
	char *ret = malloc( sizeof(char)*(length+1) );
	//Copy the part of the string that we want.
	memcpy( ret, &str[start], length );
	//Ensure it is null-terminated so it functions well.
	ret[length] = '\0';
	//Return the copied string.
	return ret;
}
