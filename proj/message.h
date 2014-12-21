#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include "defines.h"
#include "test.h"

/**
@brief Listens to any message that comes from server
@param sockfd - socket file descriptor
@param str - string to get read message
@return Returns 0 if success
*/

int listen_to(int sockfd, char *str);

/**
@brief Sends info to server (USER | PASS | etc...)
@param sockfd - socket file descriptor
@param str - string to be sent
@param code_str - USER | PASS | PASV | etc...
@param value - value
@return Returns 0 if success
*/

int send_to(int sockfd, char *str, char *code_str, char *value);

/**
@brief Awaits the response from server with an expected code
@param sockfd - socket file descriptor
@param str - string received (from listen_to)
@param code_str - expected code
@return Returns 0 when succeeds
*/

int receive_from(int sockfd, char *str, char *code_str);

#endif
