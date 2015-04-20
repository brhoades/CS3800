#include "server.h"
#include "server_utils.h"

// locks for thread positions
// UNUSED
volatile char running[MAX_CLIENTS];
volatile int sockets[MAX_CLIENTS];
pthread_mutex_t meta_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_t threads[MAX_CLIENTS];

inline void startup_accounting( )
{
  int i;
  // Show all clients as available
  for( i=0; i<MAX_CLIENTS; i++ )
    running[i] = 0;
}

inline void write_client( const int sock, const char* msg )
{
  if( write( sock, msg, sizeof(msg) ) < 0 )
  {
    perror( "Failed to write to socket" );
    pthread_exit( NULL );
  }
}

inline void new_client( const int sock )
{
  struct sockaddr_in client_addr;
  int clilen = sizeof( client_addr ), i;
  long packed[2];

  for( i=0; i<MAX_CLIENTS; i++ )
  {
    if( !running[i] )
      break;
  }

  if( i == MAX_CLIENTS )
  {
    perror( "Ran out of client slots." );
    pthread_exit( NULL );
    exit( 3 );
  }

  packed[0] = i;
  packed[1] = accept( sock, (struct sockaddr*)&client_addr, &clilen );
  if( pthread_create( &threads[i], NULL, handleClient, (void *)packed ) ) 
  {
    printf( "Thread creation failure.\n" );
    exit( 2 );
  }
  else
    printf( "New client #%i connected\n", i + 1 ); 
} 

inline void dispatch( const int source, const char* msg )
{
  int i;

  for( i=0; i<MAX_CLIENTS; i++ )
  {
    if( running[i] )
      write_client( sockets[i], msg );
  }
}

inline void client_quit( const int sock )
{

  pthread_exit( NULL );
}
