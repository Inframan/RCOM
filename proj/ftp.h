#ifndef _FTP_H_
#define _FTP_H_

#include <stdio.h>
#include "defines.h"
#include "ftp_data.h"
#include "config.h"

/**
@brief Logins the user (anonymous or not) to the server
@param sockfd - socket file descriptor
@param str - string received
@param user - username
@param password - user's password
@return Returns 0 if success
*/

int login(int sockfd, char *str, char *user, char *password);


/**
@brief If there is a path, sends the CWD (250) code to server and then waits for response
@param sockfd - socket file descriptor
@param str - buffer string
@param url - url path
@return Returns OK (0) if success
*/

int path(int sockfd, char *str, char *url_path);

/**
@brief Sends the Passive Mode code to server and then awaits for response
@param sockfd - socket file descriptor
@param str - buffer string
@return Returns OK (0) if success
*/

int passive_mode(int sockfd, char *str);

/**
@brief Saves the port
@param str - string in the format (%d, %d, %d, %d, %d, %d)
@param port - port
@return Returns OK (0) if success
*/

int port(char *str, int *port);

/**
@brief Sends the Size code to server and then awaits for response
@param sockfd - socket file descriptor
@param str - buffer string
@param filename - file name
@param filesize - file size
@return Returns OK (0) if success
*/

int file_size(int sockfd, char *str, char *filename, int *filesize);

/**
@brief Sends the Retrieve code to server
@param sockfd - socket file descriptor
@param str - buffer string
@param filename - file name
@return Returns OK (0) if success
*/

int retrieve(int sockfd, char *str, char *filename);

/**
@brief Receives the QUIT command to end connection and sends the response
@param sockfd - socket file descriptor
@param str - string received
@return Returns 0 if success
*/

int quit(int sockfd, char *str);

/**
@brief Retrieves the file specified
@param sockfd - socket file descriptor
@param filename - file name
@param filesize - file size
@return Returns OK (0) if success
*/

int retrieve_file(int sockfd, char *filename, int filesize);

/**
@brief Tests if there is an error code (http://en.wikipedia.org/wiki/List_of_FTP_server_return_codes) in the message
@note Only some errors were taken into consideration, since there are a lot of them
@param message - message string
@return Returns OK (0) if success
*/

int ftp_valid(char *message);

/**
@brief Initializes the FTP (login, path, passive mode, retrieve and filesize)
@param sockfd - socket file descriptor
@param data - data retrieved from parser
@param retr_port - retrieve port
@param filesize - file size
@return Returns OK (0) if success
*/

int ftp_init(int sockfd, FTP_Data data, int *retr_port, int *filesize);

/**
@brief Begins the file transfer
@param sockfd - socket file descriptor
@param ip - internet protocol
@param retr_port - retrieve port
@param filename - file name
@param filesize - file size
@return Returns OK (0) if success
*/

int ftp_transfer(int sockfd, char *ip, int retr_port, char *filename, int filesize);

/**
@brief Quits the FTP application (closes the socket)
@param sockfd - socket file descriptor
@return Returns OK (0) if success
*/

int ftp_quit(int sockfd);

/**
@brief Begins the FTP application
@param data - data retrieved from parser
*/

int ftp(FTP_Data data);

#endif
