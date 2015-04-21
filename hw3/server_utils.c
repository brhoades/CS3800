#include "server.h"
#include "server_utils.h"
#include "utils.h"

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
  pthread_mutex_lock( &meta_lock );
  for( i=0; i<MAX_CLIENTS; i++ )
    running[i] = 0;
  pthread_mutex_unlock( &meta_lock );
}

inline void write_client( const int sock, const char* msg )
{
  if( write( sock, msg, sizeof(msg) ) <= 0 )
    perror( "Failed to write to socket" );
}

inline void new_client( const int sock )
{
  int i;
  int packed[2];

  pthread_mutex_lock( &meta_lock );
  for( i=0; i<MAX_CLIENTS; i++ )
  {
    if( !running[i] )
      break;
  }
  pthread_mutex_unlock( &meta_lock );

  if( i == MAX_CLIENTS )
  {
    perror( "Ran out of client slots." );
    pthread_exit( NULL );
    exit( 3 );
  }

  packed[0] = i;
  packed[1] = sock;

  pthread_mutex_lock( &meta_lock );
  sockets[i] = packed[1];
  if( pthread_create( &threads[i], NULL, handleClient, (void *)packed ) ) 
  {
    printf( "Thread creation failure.\n" );
    pthread_mutex_unlock( &meta_lock );
    exit( 2 );
  }
  else
    printf( "New client #%i (sock: %i) connected\n", i, sockets[i] ); 

  running[i] = 1;
  write_client( sockets[i], "Connected" );

  pthread_mutex_unlock( &meta_lock );
} 

inline void dispatch( const int source, const char* msg )
{
  int i;

  pthread_mutex_lock( &meta_lock );
  for( i=0; i<MAX_CLIENTS; i++ )
  {
    if( running[i] && i != source )
    {
      printf( "Writing \"%s\" to %i's socket (%i)\n", msg, i, sockets[i] );
      write_client( sockets[i], msg );
    }
  }
  pthread_mutex_unlock( &meta_lock );
}

inline void client_quit( const int clientNum )
{
  char buffer[256];
  sprintf( buffer, "Client #%i quit", clientNum );
  printf( "Client #%i quit\n", clientNum );

  dispatch( clientNum, buffer );

  pthread_mutex_lock( &meta_lock );
  running[clientNum] = 0;
  pthread_mutex_unlock( &meta_lock );

  pthread_exit( NULL );
}
