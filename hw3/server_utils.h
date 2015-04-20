#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void write_client( const int sock, const char* msg );
