#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>  /* define socket */
#include <netinet/in.h>  /* define internet socket */
#include <netdb.h>       /* define internet socket */

#include "server_utils.h"

#define SERVER_PORT 9999        /* define a server port number */
#define MAX_CLIENTS 10

void *handleClient( void * );

int setupSocket( );

// locks for thread positions
// UNUSED
volatile char running[MAX_CLIENTS];
pthread_mutex_t running_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_t threads[MAX_CLIENTS];

int main( )
{
  long numClients=0;
  int sock, clilen;
  long newclient;
  struct sockaddr_in client_addr;
  int res, i;

  // Show all clients as available
  for( i=0; i<MAX_CLIENTS; i++ )
    running[i] = 0;

  sock = setupSocket( );

  while( 1 )
  {
    // sleep and wait for excitement
    printf( "Waiting for connections...\n" );
    listen( sock, 5 );
    clilen = sizeof( client_addr );


    // Accept...
    newclient = accept( sock, (struct sockaddr*)&client_addr, &clilen );
    if( pthread_create( &threads[numClients], NULL, handleClient, (void *)newclient ) ) 
    {
      printf( "Thread creation failure.\n" );
      exit( 2 );
    }
    else
      printf( "Spawned new handler thread.\n" ); 
    numClients++;
  }

  pthread_exit( NULL );
  return 0;
}

void *handleClient( void * clientNumber )
{
  long sock = (long)clientNumber;

  while( 1 )
  {
    char buffer[256];
    int res = read( sock, buffer, sizeof(buffer) ); 

    if( res < 0 )
    {
      perror( "Failed to read from socket" );
      pthread_exit( NULL );
      return NULL;
    }

    printf( "Client %i: \"%s\"\n", sock, buffer );

    // and respond
    write_client( sock, "/ACK" );
  }

  pthread_exit( NULL );
  return NULL;
}

int setupSocket( )
{
  int sock = socket( AF_INET, SOCK_STREAM, 0 ); 
  struct sockaddr_in server_addr;

  // Create a socket and check output for success
  if( sock == -1 )
  {
    perror( "Socket failed to open" );
    exit( 1 );
  }

  /* Initialize socket structure */
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons( SERVER_PORT );

  /* Now bind the host address using bind() call.*/
  if( bind( sock, (struct sockaddr *) &server_addr, sizeof( server_addr ) ) < 0 )
  {
    perror( "Failed to bind to address" );
    exit( 1 );
  }

  return sock;
}
