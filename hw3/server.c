#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>  /* define socket */
#include <netinet/in.h>  /* define internet socket */
#include <netdb.h>       /* define internet socket */

#define SERVER_PORT 9999        /* define a server port number */

int main( )
{
  int sock;
  int clilen;
  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;
  int res;

  char buffer[256];

  sock = socket( AF_INET, SOCK_STREAM, 0 ); 

  // Create a socket and check output for success
  if( sock == -1 )
  {
    perror( "Socket failed to open" );
    exit( 1 );
  }

   /* Initialize socket structure */
   //bzero( (char *) &server_addr, sizeof( server_addr ) );
   
   server_addr.sin_family = AF_INET;
   server_addr.sin_addr.s_addr = INADDR_ANY;
   server_addr.sin_port = htons( SERVER_PORT );

   /* Now bind the host address using bind() call.*/
   if( bind( sock, (struct sockaddr *) &server_addr, sizeof( server_addr ) ) < 0 )
   {
     perror( "Failed to bind to address" );
     exit( 1 );
   }

  // sleep and wait for excitement
  printf( "Waiting for connections.\n" );
  listen( sock, 5 );
  clilen = sizeof( client_addr );


  // Accept...
  sock = accept( sock, (struct sockaddr*)&client_addr, &clilen );
  res = read( sock, buffer, 255 );

  if( res < 0 )
  {
    perror( "Failed to read from socket" );
    exit( 1 );
  }

  sprintf( "This is a test: %s\n", buffer );

  // and respond
  
  res = write( sock, "Message received", 18 );

  if( res < 0 )
  {
    perror( "Failed to write to socket" );
    exit( 1 );
  }
      
  return 0;
}
