/*
        bwtest - TCP Socket Bandwith-Test

        Author: Marc Bertram
*/

#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


/* ------------------------------ */


int main (int argc, char *argv[])
{
  int s,x;
  struct sockaddr_in addr;
  char buffer[5000];

  addr.sin_addr.s_addr = inet_addr("127.0.0.1");    /* z.B. inet_addr("127.0.0.1"); */
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
  if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) == -1)
  {
    perror("connect() failed");
    return 2;
  }

  for(x=0;x<100;x++)
  {
   recv(s, buffer, sizeof(buffer) - 1, 0);
  }


  close();

  return 0;
}

