/*
 * Assignment 3 - Chatroom
 *
 * Devin Kiser
 * Billy Rhoades
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>  /* define socket */
#include <netinet/in.h>  /* define internet socket */
#include <netdb.h>       /* define internet socket */
#include <signal.h>

#include "server_utils.h"

#define SERVER_PORT 9999        /* define a server port number */
#define MAX_CLIENTS 10

void *handleClient( void * );
int setupSocket( );

