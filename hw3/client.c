/************************************************************************/ 
/*   PROGRAM NAME: client.c  (works with serverX.c)                     */ 
/*                                                                      */ 
/*   Client creates a socket to connect to Server.                      */ 
/*   When the communication established, Client writes data to server   */ 
/*   and echoes the response from Server.                               */ 
/*                                                                      */ 
/*   To run this program, first compile the server_ex.c and run it      */ 
/*   on a server machine. Then run the client program on another        */ 
/*   machine.                                                           */ 
/*                                                                      */ 
/*   COMPILE:    gcc -o client client.c -lnsl                           */ 
/*   TO RUN:     client  server-machine-name                            */ 
/*                                                                      */ 
/************************************************************************/ 
 
#include <stdio.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>  /* define socket */ 
#include <netinet/in.h>  /* define internet socket */ 
#include <netdb.h>       /* define internet socket */ 
#include <curses.h>
#include <signal.h>
#include <fcntl.h>

void draw_borders(WINDOW *screen);
void runCLI( );
static void finish(int sig);
int sock=-1;
#define SERVER_PORT 9999     /* define a server port number */ 
 
int main( int argc, char* argv[] ) 
{ 
    struct sockaddr_in server_addr = { AF_INET, htons( SERVER_PORT ) }; 
    char buf[256]; 
    struct hostent *hp; 
 
    if( argc != 2 ) 
    { 
      printf( "Usage: %s hostname\n", argv[0] ); 
      exit(1); 
    } 
 
    /* get the host */ 
    if( ( hp = gethostbyname( argv[1] ) ) == NULL ) 
    { 
      printf( "%s: %s unknown host\n", argv[0], argv[1] ); 
      exit( 1 ); 
    } 
    bcopy( hp->h_addr_list[0], (char*)&server_addr.sin_addr, hp->h_length ); 
 
    /* create a socket */ 
    if( ( sock = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 ) 
    { 
      perror( "client: socket failed" ); 
      exit( 1 ); 
    } 
 
    /* connect a socket */ 
    if( connect( sock, (struct sockaddr*)&server_addr, 
     sizeof(server_addr) ) == -1 ) 
    { 
      perror( "client: connect FAILED:" ); 
      exit( 1 ); 
    } 
 
    printf("connect() successful! will send a message to server\n"); 
    printf("Input a string:\n" ); 

    runCLI( );

    close(sock); 
    return(0); 
} 

void runCLI( )
{
  int num = 0, c = -1;
  int parent_x, parent_y;
  int score_size = 3;
  int received = 0;
  char buff[256], inputbuff[256];

  //don't block
  fcntl(sock, F_SETFL, O_NONBLOCK);

  /* initialize your non-curses data structures here */

  (void) signal(SIGINT, finish);      /* arrange interrupts to terminate */

  (void) initscr();      /* initialize the curses library */
  keypad(stdscr, TRUE);  /* enable keyboard mapping */
  //(void) nonl();         /* tell curses not to do NL->CR/NL on output */
  (void) cbreak();       /* take input chars one at a time, no wait for \n */
  (void) echo();         /* echo input - in color */
  timeout(-1);

  if (has_colors())
  {
    start_color();

    /*
     * Simple color assignment, often all we need.  Color pair 0 cannot
     * be redefined.  This example uses the same value for the color
     * pair as for the foreground color, though of course that is not
     * necessary:
     */
    init_pair(1, COLOR_RED,     COLOR_BLACK);
    init_pair(2, COLOR_GREEN,   COLOR_BLACK);
    init_pair(3, COLOR_YELLOW,  COLOR_BLACK);
    init_pair(4, COLOR_BLUE,    COLOR_BLACK);
    init_pair(5, COLOR_CYAN,    COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(7, COLOR_WHITE,   COLOR_BLACK);
  }

  noecho();
  curs_set(FALSE);
  // get our maximum window dimensions
  getmaxyx(stdscr, parent_y, parent_x);
  // set up initial windows
  WINDOW *mainbox = newwin(parent_y - score_size, parent_x, 0, 0);
  WINDOW *input = newwin(score_size, parent_x, parent_y - score_size, 0);
  keypad(input, TRUE);  /* enable keyboard mapping */
  keypad(mainbox, TRUE);  /* enable keyboard mapping */

  strcpy( buff, "" );

  do
  {
    // draw to our windows
    mvwprintw(mainbox, 0, 0, "Chat");
    mvwprintw(input, 0, 0, "Input");
    attrset(COLOR_PAIR(num % 8));
    
    //clear input
    wclear( input );

    draw_borders( mainbox );
    draw_borders( input );

    if( c != -1 )
    {
      if( c == '\n' )
      {
    //FIXME: we should see our own message on completion
        /*
        while( 1 ) 
        { 
          int res = scanf(" %[^\n]", &buf); 
          if( res <= 0 )
            break;
          else
          {
            write(sock, buff, sizeof(buff)); 
            read(soc, buff, sizeof(buff)); 
            printf("SERVER: %s\n", buff); 
          }
        }
          */
        buff[num+1] = '\0';
        write( sock, buff, sizeof(buff) );
        // GET ACK
        num = 0;
        buff[0] = '\0';
      }
      else if( c == KEY_BACKSPACE )
      {
        if( num > 0 )
        {
          buff[num] = '\0';
          num--;
        }
      }
      else if( num >= 254 )
      {
        buff[254] = c;
        buff[255] = '\0';
        num = 254;
      }
      else
      {
        buff[num] = c;
        num++;
        buff[num] = '\0';
      }
    }

    mvwprintw(input, 1, 2, "%s (%i/%i)", buff, num, strlen(buff));
    // refresh each window
    read(sock, inputbuff, sizeof(inputbuff)); 
    if( strlen( inputbuff ) > 0 )
    {
      mvwprintw(mainbox, 1+received, 2, "msg: '%s' (%i)", inputbuff);
      received++;
      inputbuff[0]='\0';
    }
    else
    {
      //mvwprintw(mainbox, 1, 2, "msg: '%s'", inputbuff);
    }
    wrefresh(mainbox);
    wrefresh(input);
  }
  while( ( c = wgetch( input ) ) );

  // clean up
  delwin(mainbox);
  delwin(input);

  finish(0);
} 

static void finish(int sig)
{
  endwin();
  pthread_exit( NULL );

  /* do your non-curses wrapup here */

  exit(0);
}

void draw_borders(WINDOW *screen) {
  int x, y, i;
  getmaxyx(screen, y, x);
  // 4 corners
  mvwprintw(screen, 0, 0, "+");
  mvwprintw(screen, y - 1, 0, "+");
  mvwprintw(screen, 0, x - 1, "+");
  mvwprintw(screen, y - 1, x - 1, "+");
  // sides
  for (i = 1; i < (y - 1); i++) {
    mvwprintw(screen, i, 0, "|");
    mvwprintw(screen, i, x - 1, "|");
  }
  // top and bottom
  for (i = 1; i < (x - 1); i++) {
    mvwprintw(screen, 0, i, "-");
    mvwprintw(screen, y - 1, i, "-");
  }
}
