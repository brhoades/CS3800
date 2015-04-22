#include "client.h"

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

void signalhandler(const int sig)
{
	//YA.... We're just gonna do nothing here....
	
}
