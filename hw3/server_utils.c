#include "server_utils.h"

inline void write_client( const int sock, const char* msg )
{
  if( write( sock, msg, sizeof(msg) ) < 0 )
  {
    perror( "Failed to write to socket" );
    pthread_exit( NULL );
  }
}
