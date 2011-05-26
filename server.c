/*
        bwtest - TCP Socket Bandwith-Test

        Author: Marc Bertram
*/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

/* ------------------------------ */

void client_behandlung(int client)
{
    struct timeval tv;

   gettimeofday(&tv, 0);

    printf ("%d.%d\n", tv.tv_sec, tv.tv_usec);
    
  
    
 
}

/* ------------------------------ */
int main (int argc, char *argv[])
{
  int s, c;
  struct sockaddr_in addr;
  struct sockaddr_in cli;
  int cli_size;


  addr.sin_addr.s_addr = INADDR_ANY;	/* z.B. inet_addr("127.0.0.1"); */
  addr.sin_port = htons(5001); 		/* z.B. htons(80);              */
  addr.sin_family = AF_INET;

  // Create Socket
  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s == -1)
  {
    perror("socket() failed");
    return 1;
  }

  // Bind Socket
  if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) == -1)
  {
    perror("bind() failed");
    return 2;
  }

  // Listen
  if (listen(s, 3) == -1)
  {
    perror("listen () failed");
    return 3;
  }

  // Accept Client Connection
  cli_size = sizeof(cli);
  //c = accept(s, &cli, &cli_size);
  for(;;)
  {
    c = accept(s, (struct sockaddr *)&cli, &cli_size);
    printf("Verbindung von %s\n", inet_ntoa(cli.sin_addr));
    client_behandlung(c);
    close(c);
  }

  close();

  return 0;
}

