void startup_accounting( );
void dispatch( const int source, const char* msg );
void new_client( const int sock );
void write_client( const int sock, const char* msg );
void client_quit( const int sock );
