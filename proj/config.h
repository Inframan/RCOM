#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdio.h>
#include <netdb.h> 
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include "defines.h"

/**
@brief Establishes the connection based on hostname	
@param hostname - hostname
@return Returns the IP address if succeeded
*/

char* getIP(char *hostname);

/**	
@brief Tries to assign the socket file descriptor (asocket -> assign socket)
@param ip - IP address that comes from getIP function
@param port - port that comes from parsing
@return sockfd if success
*/

int asocket(char *ip, int port);

#endif
