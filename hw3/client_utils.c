/*
 * Assignment 3 - Chatroom
 *
 * Devin Kiser
 * Billy Rhoades
*/
#include "client.h"
#include "client_utils.h"

void get_message( const char* msg, WINDOW* mainbox, int* cnt )
{
  int y, x;
  getmaxyx( mainbox, y, x );

  if( *cnt < y-2 )
    (*cnt)++;
  else
    wscrl( mainbox, 1 );

  mvwprintw(mainbox, *cnt, 2, "%s", msg );
  refresh( );
  //FIXME: word wrap
}

void endclisig(const int sig)
{
  endcli( );	
}

inline void endcli( )
{
  endwin();

  exit( 0 );
}
