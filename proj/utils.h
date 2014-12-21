/**
@brief Finds the nth element of a string
@param str - string
@param c - character
@param start - starting position
@param nth - nth position of character
@return Returns the position or NOT_FOUND (-1) otherwise
*/

int find_nth(char *str, char c, int start, int nth){

	if(start >= strlen(str)) return NOT_FOUND;

	for(; str[start] != NULL_CHAR; start++){
		if(str[start] == c){
			if(nth == ONE) return start;
			else nth--;
		}
	}

	return NOT_FOUND;
}

/**
@brief Copies a string orig to dest, from start to end
@param orig - origin string
@param start - starting position
@param end - ending position
@return Returns the destination string
*/

char* str_copy(char *orig, int start, int end){

	int length = end - start;
	char *dest = malloc(length + ONE);

	memcpy(dest, &orig[start], length);
	dest[length] = NULL_CHAR;

	return dest;
}
