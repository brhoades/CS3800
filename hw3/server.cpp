/*/////////////////////////////////////////////////////////////////////////////
COMPILATION:  g++ -o server server.cpp -lpthread -D_REENTRANT
//////////////////////////////////////////////////////////////////////////////*/

#include <pthread.h>
#include <iostream>
#include <signal.h> 
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>  /* define socket */
#include <netinet/in.h>  /* define internet socket */
#include <netdb.h>       /* define internet socket */
#include "header.h"

#define MAX_CLIENT 10  // max number of clients at one time
 
#define SEND_ALL -1

void signal_handler( int sig);
void ISvalid(int argc, char * argv[], 
    struct sockaddr_in * server_addr);
void * client_handler(void * ptr);
void allClientWrite(int netSock, char * msg);
void cleanClient(int netSock);

pthread_mutex_t lock;
    
int array_of_clients[MAX_CLIENT];
int no_of_clients_now;  
int sd;
pthread_t client_thread[MAX_CLIENT];
 

void * client_handler(void * ptr)
{
    int netSock = *((int*)ptr);
    int num_read;
    char username[MAX_CLIENTNAME];
    char buf[MAX_MSG_LEN];
    char sbuf[MAX_BUFFER_LEN];
    
    //The client will send a message containing its username first.
    if ( read( netSock, username, MAX_CLIENTNAME) != EOF)
    {
        //The client is connecting.
        strcpy(sbuf, ">> User '");
        strcat(sbuf, username);
        strcat(sbuf, "' connected.");
        allClientWrite(SEND_ALL, sbuf);
        printf("%s\n", sbuf);
    }
        
    //Read input from client until the connection is closed.
    while( read( netSock, buf, sizeof(buf)) > 0 )
    {
        if( strcmp(buf, "/exit") && strcmp(buf, "/quit") && 
            strcmp(buf, "/part") )
        {
            strcpy(sbuf, username);
            strcat(sbuf, " : ");
            strcat(sbuf, buf);
            //Echo the message to everyone but this client.
            allClientWrite(netSock, sbuf);
            //Print the message to the server's screen.
            printf("%s\n", sbuf);
            memset(buf, '\0', sizeof(buf));
        }
        else
        {
            //The client is exiting.
            printf("Client %s is exiting.", username);
            break;
        }
    }
    
    //The client is exiting so inform remaining clients.
    strncpy(sbuf, ">> User '", sizeof(">> User '"));
    strcat(sbuf, username);
    strcat(sbuf, "' left.");
    allClientWrite(netSock, sbuf);
    printf("%s\n", sbuf);
    
    //Clean up our created arrays (remove this client).
    cleanClient(netSock);
             
    pthread_exit( ptr);
}

//writes to all clients
void allClientWrite(int netSock, char * msg)
{
    pthread_mutex_lock( &lock );
    for (int i = 0; i < no_of_clients_now; i++)
    {
        if( array_of_clients[i] != netSock )
            write( array_of_clients[i], msg, strlen(msg)+1 );
    }
        
    pthread_mutex_unlock( &lock );

}

 
 // ctrl-C handler for server
void signal_handler(int sig) 
{    
    //Send message to all clients.
    printf("Ctrl-C recieved, server is shutting down in 10 seconds....\n");
    allClientWrite(SEND_ALL, ">> Server is shutting down in 10 seconds....");
    sleep(10);  //wait 10 seconds
   
    exitAll = true;
    
    //Clear out all threads, close connections.
    for (int i = 0; i < no_of_clients_now; i++)
    {
        pthread_cancel( client_thread[i],0 );
        close( array_of_clients[i] );
    };
    
    printf("GoodBye...\n");
    
    close(sd);
}

// Assigns valid port
void ISvalid(int argc, char * argv[], struct sockaddr_in * server_addr)
{
    server_addr->sin_port = htons( atoi( argv[1]));
    
}

// Cleaning clients on exit for further use of the port
void cleanClient(int netSock)
{
    pthread_mutex_lock(&lock);
    int i = 0;
    
    //Find the position needing to be emptied in the array.
    while ( i < no_of_clients_now && array_of_clients[i] != netSock)
    {
        i++;
    };
    
    //Shift everything over by 1, right to left, overwriting the data to be
    // deleted.
    for(int j=i; j<(no_of_clients_now-1); j++)
    {
        array_of_clients[j] = array_of_clients[j+1];
        client_thread[j] = client_thread[j+1];        
    }
    no_of_clients_now--;
    
    pthread_mutex_unlock(&lock);
    
    return;
}



//main
int main( int argc, char* argv[]) 
{
    int netSock;
    int client_length = sizeof(struct sockaddr_in );
    struct sockaddr_in server_addr = { AF_INET, htons( SERVER_PORT ) };
    struct sockaddr_in client_addr = { AF_INET };
    
    exitAll = false;
    no_of_clients_now = 0; //initialize client connection count
    
    //change cntl-c signal handler
    signal( SIGINT, signal_handler); 
    
    // Check the proper number of parameters and if a second parameter is 
    // specified that it is in the correct range 
	if( argc > 2)
    {
        fprintf( stderr, "Usage: %s or\n", argv[0]);
        fprintf( stderr, "Usage: %s SERVER_PORT\n", argv[0]);
        exit( 1 );
    }
      
    if ( ( argc == 2) && ( atoi( argv[1]) < 1024 || atoi( argv[1]) > 65535))
    {	// Check server port number 
        fprintf( stderr, "server: SERVER_PORT must be >= 1024 and <= 65535 \n");
        exit( 1 );
    }
    // If user specifies server port, assign user server port number
    else if ( argc == 2)
    {
    ISvalid( argc, argv, &server_addr);
	}
    // Create socket, if socket fails exit program 
    if( ( sd = socket( AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror( "server: socket failed");
        exit( 1);
    }
    
    // bind the socket to an internet port, if failure exit
    if( bind(sd, (struct sockaddr*)&server_addr, sizeof( server_addr)) == -1 )
    {
        perror( "server: bind failed" );
        exit( 1 );
    }

    // listen for clients, if failure exit
    if( listen( sd, MAX_CLIENT ) == -1 )
    {
        perror( "server: listen failed" );
        exit( 1 );
    }

    printf("server: listening for maximum 10 clients to establish a connection.....\n");

    //Loop, accepting new connections until exit flag received from cntl-c
    while( !exitAll )
    {
        //Ensure that we don't exceed the maximum number of connections.
        if (no_of_clients_now < MAX_CLIENT)
        {
            //Accept the connection
            if ( (netSock = accept( sd, (struct sockaddr*)&client_addr, 
                (socklen_t*)&client_length)) > 0 )
            {
                pthread_mutex_lock( &lock );
                array_of_clients[no_of_clients_now] = netSock;
            
                //Try to create a thread for this client
                if ( pthread_create( &client_thread[no_of_clients_now], NULL,
                     client_handler, &array_of_clients[no_of_clients_now]))
                {
                    perror( "server: UNABLE TO CREATE THREAD\n");
                    exit( 1 );
                }
            
                no_of_clients_now++;
                pthread_mutex_unlock( &lock );
            }    
        }
    };

    unlink((char *)&server_addr.sin_addr);

   return(0);
} // End of main





