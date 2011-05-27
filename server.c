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
#include <arpa/inet.h>

#define BUFFER_SIZE 1500 

/* global variables */

int debug = 0;
int listenport = 21000;

/* --------- prototypes --------- */

void client_behandlung(int client);

/* ------------------------------ */

void client_behandlung(int client)
{
  struct timeval tim;
  double t1, t2;
  int x, y, recv_bytes, send_bytes;
  char send_msg[] ="TEST\0";
  char recv_msg[BUFFER_SIZE];
  char control_msg[10];

  while (1)
  {
    recv(client, control_msg, sizeof(control_msg), 0);
    //printf ("%s", control_msg);
    if(strstr (control_msg, "exit") != 0)
      return;
    gettimeofday(&tim, NULL);
    t1=tim.tv_sec+(tim.tv_usec/1000000.0);
    send_bytes = send(client, send_msg, strlen(send_msg), 0);
    recv_bytes = recv(client, recv_msg, sizeof(recv_msg), 0);
    gettimeofday(&tim, NULL);
    t2=tim.tv_sec+(tim.tv_usec/1000000.0);
    printf ("S-Size: %d\tR-Size: %d\tDauer: %.6lf seconds\n", send_bytes, recv_bytes, t2-t1);
    printf ("Send: %s\tReceive: %s\n", send_msg, recv_msg);
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
  addr.sin_port = htons(listenport); 		/* z.B. htons(80);              */
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

