/*
 * Assignment 3 - Chatroom
 *
 * Devin Kiser
 * Billy Rhoades
*/
/***************************
  General tools for client and server
 **************************/

#include "utils.h"

inline int isSocketAlive( int sock )
{ 
  int error = 0;
  socklen_t len = sizeof( error );
  return( !setsockopt( sock, SOL_SOCKET, SO_ERROR, &error, &len ) );
}
