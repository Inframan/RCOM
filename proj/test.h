#ifndef _TEST_H_
#define _TEST_H_

#include <errno.h>
#include "defines.h"

/**
@brief Tests if it could receive anything
@param sockfd - socket file descriptor
@param str - buffer string
@return Returns OK (0) if success
*/

int test_receive(int sockfd, char *str);

/**
@brief Checks if the message has the expected code
@param str - string received (message)
@param code_str - expected code
@return Returns ONE (1 -> true) if success (code exists) and 0 (false) otherwise
*/

int test_response(char *str, char *code_str);

#endif
