#include "server.h"

// This thread becomes a listner
int main( )
{
  int sock;

  startup_accounting( );

  sock = setupSocket( );

  while( 1 )
  {
    // sleep and wait for excitement
    printf( "Waiting for connections...\n" );
    listen( sock, 5 );

    new_client( sock );
  }

  pthread_exit( NULL );
  return 0;
}

void *handleClient( void * packed )
{
  long clientNum = ((long*)packed)[0];
  long sock = ((long*)packed)[1];

  while( 1 )
  {
    char buffer[256];
    int res = read( sock, buffer, sizeof(buffer) ); 

    if( res < 0 )
    {
      perror( "Failed to read from socket" );
      client_quit( clientNum );
      return NULL;
    }

    printf( "Client %i: \"%s\"\n", sock, buffer );

    if( !strcmp( buffer, "/quit" ) )
      client_quit( clientNum );

    // and respond
    write_client( sock, "/ACK" );

    // and tell everyone else
    dispatch( sock, buffer );
  }

  client_quit( clientNum );
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
