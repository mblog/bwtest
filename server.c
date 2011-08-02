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

#define BUFFER_SIZE 5000 

/* global variables */

int debug = 0;
int listenport = 21000;

/* --------- prototypes --------- */

void client_behandlung(int client);

void send_ping (int client);

/* ------------------------------ */

void client_behandlung(int client)
{
  struct timeval tim;
  double t1, t2, timesum;
  int x, y, recv_bytes, send_bytes;
  char send_msg[BUFFER_SIZE];
  char recv_msg[BUFFER_SIZE];
  char control_msg[10];

  //while (1)
  for (x = 1; x <= 14; x++)
  {
    timesum = 0;
    //recv(client, control_msg, sizeof(control_msg), 0);
    //printf ("%s", control_msg);
    //if(strstr (control_msg, "exit") != 0)
    //  break;
    for (y=0;y<10;y++)
    {
      gettimeofday(&tim, NULL);
      t1=tim.tv_sec+(tim.tv_usec/1000000.0);
      send_bytes = send(client, send_msg, x*100, 0);
      recv_bytes = recv(client, recv_msg, sizeof(recv_msg), 0);
      gettimeofday(&tim, NULL);
      t2=tim.tv_sec+(tim.tv_usec/1000000.0);
      recv_msg[recv_bytes] = '\0';
      //printf ("S-Size: %d\tR-Size: %d\tDauer: %.6lf seconds\n", send_bytes, recv_bytes, t2-t1);
      if (recv_bytes < 0)
         return;
      timesum += t2-t1;
      //printf ("Send: %s\tReceive: %s\n", send_msg, recv_msg);
    }
    printf("Paketgröße: %d \t-\tDauer: %.6lf seconds\n", x*100, timesum/10);
  }
}

void send_ping (int client)
{
  
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

  /* Disable the Nagle (TCP No Delay) algorithm */
  int flag = 1;
  int ret;
  ret = setsockopt( s, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag) );
  if (ret == -1) {
    printf("Couldn't setsockopt(TCP_NODELAY)\n");
    exit( EXIT_FAILURE );
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

