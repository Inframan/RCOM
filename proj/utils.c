#include "utils.h"

int find_nth(char *str, char c, int start, int nth){

	if(start >= strlen(str)) 
		return NOT_FOUND;

	int i = start;

	while(str[i] != NULL_CHAR){

		if(str[i] == c){
			if(nth == ONE) return i;
			else nth--;
		}

		i++;
	}
		
	return NOT_FOUND;
}

char* str_cpy(char *orig, int start, int end){

	int i = start, j;
	char *dest = malloc(end - start + ONE);
	
	for(j = ZERO; i < end; i++, j++)
		dest[j] = orig[i];
	
	dest[j] = NULL_CHAR;

	return dest;

}
