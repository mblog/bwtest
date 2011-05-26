/*
        bwtest - TCP Socket Bandwith-Test

        Author: Marc Bertram
*/

#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

/* ------------------------------ */

void client_behandlung(int client)
{
  struct timeval tim;
  double t1, t2;
  int x, y, bytes;
  char buffer[] = "ABCDEFGHIJKLMOPQRSTUVWXYZ";
  char puffer[5000] = "";

  //gettimeofday(&tv, 0);

  //printf ("%d.%d\n", tv.tv_sec, tv.tv_usec);
  for(x=0; x<100;x++)
  {
    strncat(puffer, buffer,5000-strlen(buffer)+1);
    //printf ("Puffer: %s\n", puffer);
    gettimeofday(&tim, NULL);
    double t1=tim.tv_sec+(tim.tv_usec/1000000.0);
    bytes = send(client, puffer, strlen(puffer), 0);
    gettimeofday(&tim, NULL);
    double t2=tim.tv_sec+(tim.tv_usec/1000000.0);
    printf ("Packetgröße: %d\tDauer: %.6lf seconds\n", bytes, t2-t1);
  }
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

