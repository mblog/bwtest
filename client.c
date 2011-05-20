/*
        bwtest - TCP Socket Bandwith-Test

        Author: Marc Bertram
*/

#include <sys/types.h>
#include <sys/socket.h>

/* ------------------------------ */


int main (int argc, char *argv[])
{
  int s;
  struct sockaddr_in addr;

  addr.sin_addr.s_addr = inet_addr("78.47.146.248");    /* z.B. inet_addr("127.0.0.1"); */
  addr.sin_port = htons(5001);             /* z.B. htons(80);              */
  addr.sin_family = AF_INET;

  // Create Socket
  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s == -1)
  {
    perror("socket() failed");
    return 1;
  }

  // Connect to Server
  if (connect(s, &addr, sizeof(addr)) == -1)
  {
    perror("connect() failed");
    return 2;
  }

  


  close();

  return 0;
}

