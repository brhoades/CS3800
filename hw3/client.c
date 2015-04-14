
/*********************************************************************************/
/*   PROGRAM: client-template for the assignment                                */
/*                                                                                                              */
/*   Client creates a socket to connect to Server.                                     */
/*   When the communication established, Client writes data to server   */
/*   and echoes the response from Server.                                              */
/*                                                                                                              */
/*   gcc -o client client.c -lpthread -D_REENTRANT                                */
/*                                                                                                              */
/*********************************************************************************/


int main()
{
  socket stuff (including gethostbyname(), bcopy(), socket())

  connect
  
  introduce the client to server by passing the nickname;

  read the echoed message;

  create a thread for reading messages;

  while(still input to keyboard)
  {
    write input to socket
  }

  close stuff ...
}




void* reader(void* arg)
{
    get FD
    while (read(FD) > 0)
    {
       print to screen what is read
    }
    close(FD)
}
 
