#include "client.h"
#include "client_utils.h"

int sock=-1;
//For some reason, uncommented this line, wont compile
//int Error = 0;
char nickname[MAX_NICKNAME];
char exitMsg[32];

int main( int argc, char* argv[] ) 
{ 
    //At this point we can handle the CTRL+C
    signal( SIGINT, signalhandler );

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
    printf("Please Input a Nickname:   " );
    gets(nickname);
    while (strlen(nickname) > 14)
    {
      printf("\nNickname Max is %i chars\n", MAX_NICKNAME);
      printf("Please Input a Nickname:   " );
      gets(nickname);
    }


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
  nodelay(stdscr, TRUE); // don't block on getch

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
  WINDOW *mainbox = newwin(parent_y - score_size-2, parent_x-1, 1, 1); // we are off by one to allow borders outside of our window
  draw_borders_outside(parent_y - score_size, parent_x);
  mvprintw( 0, TITLE_START, "Chat" );
  WINDOW *input = newwin(score_size, parent_x, parent_y - score_size, 0);
  scrollok(mainbox, 1);
  keypad(input, TRUE);  /* enable keyboard mapping */
  keypad(mainbox, TRUE);  /* enable keyboard mapping */

  strcpy( buff, "" );
  strcpy( inputbuff, "" );

  attrset(COLOR_PAIR(num % 8));

  draw_borders( input );

  // draw to our windows
  mvwprintw( input, 0, TITLE_START, "Input" );

  wrefresh( mainbox );
  wrefresh( input );

  //Initial Connected Message
  char conMsg[255];
  strcpy( conMsg, nickname );
  strcat(conMsg, " has connected to the chat.");
  write( sock, conMsg, strlen(conMsg)+1 );
  get_message( conMsg, mainbox, &received );


  do
  {
    //Format the Message to server
    char socketOut[255-MAX_NICKNAME];
    strcpy(socketOut, nickname);
    if (strlen(nickname) < 6)
      strcat(socketOut, "\t\t:  ");
    else if (strlen(nickname) >= 6 && strlen(nickname) < 14)
      strcat(socketOut, "\t:  ");
    else
      strcat(socketOut, ":  ");


    draw_borders( input );
    //draw_borders( mainbox );

    // draw to our windows
    //mvwprintw( mainbox, 0, 0, "Chat" );
    mvwprintw( input, 0, TITLE_START, "Input" );

    //If the user inputs anything 
    if( c != -1 && c != ERR )
    {

      //When user hits enter
      if( c == '\n' )
      {
        // this catches sending newlines to the server.
        if( strlen( buff ) == 0 )
        {
          c = -1;
          continue;
        }
        //If entering a command
        else if (buff[0] == '/' && buff[1] != '/')
        {
          if (!strcmp("/quit",buff) || !strcmp("/part",buff) || !strcmp("/exit",buff))
          {
            strcpy(socketOut, "");
            strcat(socketOut, nickname);
            strcat(socketOut, " has left the chat");
            write( sock, socketOut, strlen(socketOut)+1 );

            //cleanup
            delwin(mainbox);
            delwin(input);

            finish(0);
          }
        }

        //Send the Messsage to the Server
        strcat(socketOut, buff);
        write( sock, socketOut, strlen(socketOut)+1 );

        //Format the Window Output
        char winOut[255-7];
        strcpy(winOut, "Me\t\t:  ");
        strcat(winOut, buff);

        //Write Message to this clients window
        get_message( winOut, mainbox, &received );

        // GET ACK
        num = 0;
        buff[0] = '\0';
      }
      //Remove one char from input box
      else if( c == KEY_DC || c == KEY_BACKSPACE )
      {
        if( num > 0 )
        {
          num--;
          buff[num] = '\0';
        }
      }
      //When input too large
      else if( num >= (254 - MAX_NICKNAME) )
      {
        buff[254 - MAX_NICKNAME] = c;
        buff[255 - MAX_NICKNAME] = '\0';
        num = 254 - MAX_NICKNAME;
      }
      //Add the character to input window
      else
      {
        buff[num] = c;
        num++;
        buff[num] = '\0';
      }

      //Clear input and write the current input string
      wclear( input );
      mvwprintw(input, 1, 2, "%s", buff, num, strlen(buff));
    }

    read(sock, inputbuff, sizeof(inputbuff)); 
    if( strlen( inputbuff ) > 0 )
    {
      get_message( inputbuff, mainbox, &received );
      strcpy( inputbuff, "" );
    }
    /*
    if (exitError)
    {
      strcpy(exitMsg, "Please Use /part /quit /exit");
      get_message( exitMsg, mainbox, &received );
      exitError = 0;
    }
    */

    wrefresh( mainbox );
    wrefresh( input );
    refresh( );
    usleep(250); // refresh this often, ms/
  }
  while( ( c = getch( ) ) );

  /*
  // clean up
  delwin(mainbox);
  delwin(input);

  finish(0);
  */
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

void draw_borders_outside(const int y, const int x) {
  int i;
  // 4 corners
  mvprintw( 0, 0, "+");
  mvprintw( y - 1, 0, "+");
  mvprintw( 0, x - 1, "+");
  mvprintw( y - 1, x - 1, "+");
  // sides
  for (i = 1; i < (y - 1); i++) {
    mvprintw( i, 0, "|");
    mvprintw( i, x - 1, "|");
  }
  // top and bottom
  for (i = 1; i < (x - 1); i++) {
    mvprintw(0, i, "-");
    mvprintw(y - 1, i, "-");
  }
}
