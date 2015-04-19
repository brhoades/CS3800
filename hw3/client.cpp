/*///////////////////////////////////////////////////////////////////////////////                                                                     
COMPILATION:  g++ -o client client.cpp -lpthread -D_REENTRANT       
//////////////////////////////////////////////////////////////////////////////*/

#include <pthread.h>
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>  // define socket 
#include <netinet/in.h>  // define internet socket 
#include <netdb.h>       // define internet socket 
#include "header.h"

using namespace std;


void signal_handler( int sig);
void * fromServer(void * ptr);
void * fromUser(void * ptr);
void get_hostname(struct hostent ** hp, char * username);
void ISvalid(int argc, char * argv[], 
    struct sockaddr_in * server_addr);

struct clients
{
    int sd;
    char username[MAX_CLIENTNAME];
};

struct clients client_info;
pthread_t read_thrd, write_thrd;
pthread_mutex_t lock;

// Gets server hostname to connect to
void get_hostname(struct hostent ** hp, char * username)
{
   char hostname[ MAX_HOSTNAME];
   
   printf( "Enter server hostname: ");
   cin.getline( hostname, MAX_HOSTNAME);
   
   // Check if server hostname exists else print error message
   if ( ( *hp = gethostbyname(hostname)) == NULL )
   {
      perror( "UNKNOWN HOST\n");
      exit( 1 );
   }
   
   printf( "Enter a username: ");
   cin.getline( username, MAX_CLIENTNAME);
}

// interrupt handler to prevent cntl-c from terminating the program
void signal_handler(int sig) 
{    
   printf("client: please type /exit, /quit or /part to exit\n");
   
   return;
}


void ISvalid(int argc, char * argv[], struct sockaddr_in * server_addr)
{
    server_addr->sin_port = htons( atoi( argv[1]));
   
}


// thread to catch and print(on client) data echoed from server . If the server shuts,
// this thread will terminate the write (fromUser) thread to exit the 
// program
void * fromServer(void * ptr)
{
    clients * readPtr = (clients*)ptr;
    char buf[MAX_BUFFER_LEN];
        
    // continue while the socket is open and data is present in the read buffer   
    while( read(readPtr->sd, buf, sizeof( buf)) > 0 )
    {        
        printf("%s\n", buf);  //assumes the information is correctly formated
        memset(buf, '\0', sizeof(buf));
    }
    
    //Stop writing
    pthread_cancel( write_thrd, 0 );    
    pthread_exit( ptr);
}

// thread to catch and send (to server) user input.  If the
// user types an exit condition, the thread closes the read (fromServer) thread,
// to exit the program.
void * fromUser(void * ptr)
{
    char buf[MAX_MSG_LEN];
    
    // continue getting user input from terminal until exit condition received
    while (  strcmp(buf, "/exit") && strcmp(buf, "/quit") && 
            strcmp(buf, "/part"))
    {
        cin.getline(buf, MAX_MSG_LEN);
        
        if( strcmp(buf, "/exit") && strcmp(buf, "/quit") && 
            strcmp(buf, "/part") )
        {
            write(client_info.sd, buf, strlen(buf) + 1);
        }
        
    };
    
    //Stop reading
    pthread_cancel( read_thrd, 0 );

    pthread_exit( ptr);
}

int main( int argc, char * argv[])
{
    int sd;
    char buf[MAX_MSG_LEN];
    struct sockaddr_in server_addr = { AF_INET, htons( SERVER_PORT ) };
    struct hostent * host;   
      
    if( argc > 2) // checks for arguments
    {
        fprintf( stderr, "Expected Inputs: %s or\n", argv[0]);
        fprintf( stderr, "Expected Inputs: %s SERVER_PORT\n", argv[0]);
        exit( 1 );
    }
       
    
    if ( ( argc == 2) && ( atoi( argv[1]) < 1024 || atoi( argv[1]) > 65535))// Check server port number
    {
        fprintf( stderr, "client: SERVER_PORT must be >= 1024 and <= 65535 \n");
        exit( 1 );
    }
    
    else if ( argc == 2) // If user specifies server port, assign user server port number
    {
    ISvalid( argc, argv, &server_addr);
    }
    
    get_hostname(&host, client_info.username); // get the server hostname and the client's username
         
    memcpy(&server_addr.sin_addr, host->h_addr, host->h_length ); // Copy IP address from host to server_addr
    
    if( ( client_info.sd = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 ) //create socket
    {
        perror( "client: SOCKET FAILED\n" );
        exit( 1 );
    }
    
    if( connect( client_info.sd, (struct sockaddr*)&server_addr, 
        sizeof(server_addr) ) == -1 )	// Connect to socket
    {
        perror( "client: connect FAILED:" );
        exit( 1 );
    }
     
    printf("You are successfully connected!!\n");
    signal( SIGINT, signal_handler); //Ctrl-C handler for client
    
    if ( pthread_create( &read_thrd, NULL, fromServer, &client_info))// child threads to accepting user input and check for exit conditions
    {
        perror( "client: UNABLE TO CREATE THREAD\n");
        exit( 1 );
    }
    
    if ( pthread_create( &write_thrd, NULL, fromUser, &client_info))// child thread echoing server messages
    {
        perror( "client: UNABLE TO CREATE THREAD\n");
        exit( 1 );
    }

    //Send the nickname of the client as the first message.
    write(client_info.sd, client_info.username, strlen(client_info.username) + 1);
       
    pthread_join(read_thrd, NULL);
    pthread_join(write_thrd, NULL);
    
    // Close socket with server
    close(client_info.sd);
    
    return(0);
}

