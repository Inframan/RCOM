
#ifndef _UTILS_H_
#define _UTILS_H_

#include "defines.h"

/**
@brief Finds the nth element of a string
@param str - string
@param c - character
@param start - starting position
@param nth - nth position of character
@return Returns the position or NOT_FOUND (-1) otherwise
*/

int find_nth(char *str, char c, int start, int nth);

/**
@brief Copies a string orig to dest, from start to end
@param orig - origin string
@param start - starting position
@param end - ending position
@return Returns the destination string
*/

char* str_cpy(char *orig, int start, int end);

#endif
