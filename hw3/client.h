/*
 * Assignment 3 - Chatroom
 *
 * Devin Kiser
 * Billy Rhoades

 Compilation: run the command "make"
*/
#include <stdio.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>  /* define socket */ 
#include <netinet/in.h>  /* define internet socket */ 
#include <netdb.h>       /* define internet socket */ 
#include <curses.h>
#include <signal.h>
#include <fcntl.h>

void draw_borders(WINDOW *screen);
void runCLI( );
static void finish(int sig);
void draw_borders_outside(const int x, const int y);

#define SERVER_PORT 9999     /* define a server port number */ 
#define TITLE_START 2
#define MAX_NICKNAME 14
#define MAX_MESSAGE 256
#define THINK_RATE 250       // How often we poll for user input and server messages
 
