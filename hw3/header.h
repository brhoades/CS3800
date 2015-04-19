#define SERVER_PORT 9999  // DEFAULT server port number 
#define MAX_HOSTNAME 128  // max number of characters in server hostname
#define MAX_CLIENTNAME 128  // max number of character is client's username
#define MAX_MSG_LEN 1024  // max number of characters of user data
#define MAX_BUFFER_LEN 1155  
char buf[MAX_BUFFER_LEN];
bool exitAll=false;
void pthread_cancel(pthread_t thread, int n)
{
pthread_cancel(thread);
printf("Exitted!!\n");
return;
}
