/*
 * Assignment 3 - Chatroom
 *
 * Devin Kiser
 * Billy Rhoades

 Compilation: run the command "make"
*/
#include "server.h"
#include "client.h" // this pulls our constants like max message length

int sock;

// This thread becomes a listner
int main( )
{
  // Ignore SIGPIPE for now.... causing termination
  signal( SIGPIPE, SIG_IGN );
  signal( SIGINT, signalhandler );

  startup_accounting( );

  sock = setupSocket( );

  // sleep and wait for excitement
  printf( "Waiting for connections...\n" );

  while( 1 )
  {
    struct sockaddr_in client_addr;
    int clilen = sizeof( client_addr );

    listen( sock, 5 );
    new_client( accept( sock, (struct sockaddr*)&client_addr, &clilen ) ); // accept blocks
  }

  printf( "Done!\n" );

  closeSocket( );
  pthread_exit( NULL );
  return 0;
}

void *handleClient( void * packed )
{
  int clientNum = ((int*)packed)[0];
  int sock = ((int*)packed)[1];

  while( 1 )
  {
    char buffer[MAX_MESSAGE];
    int res = read( sock, buffer, sizeof(buffer) ); 

    if( res <= 0 )
    {
      if( res != 0 )
        perror( "Failed to read from socket" );
      client_quit( clientNum );
      return NULL;
    }

    printf( "Client %i: \"%s\"\n", clientNum, buffer );

    // and tell everyone else
    dispatch( clientNum, buffer );
  }

  client_quit( clientNum );
  closeSocket( );
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

inline void closeSocket( )
{
  close( sock );
}
