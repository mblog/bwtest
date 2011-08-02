/*
        bwtest - TCP Socket Bandwith-Test

        Author: Marc Bertram
*/

#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#define BUFFER_SIZE 5000

/* global variables */

int debug = 0;
int serverport = 21000;


/* ------------------------------ */

void ping_back (int server)
{
  char recv_msg[BUFFER_SIZE];
  int x, y, bytes;
  char stop_msg[] = "exit\0";
  char control_msg[] ="start\0";

  //for (x=1; x<=14; x++)
  while((bytes = recv(server, recv_msg, sizeof(recv_msg), 0)) > 0)
  {
    //for (y = 0; y < 4; y++)
    //{
      //send(server, control_msg, strlen(control_msg), 0);
      //bytes = recv(server, recv_msg, sizeof(recv_msg), 0);
      recv_msg[bytes] = '\0';
      send(server, recv_msg, bytes, 0);
    //}
  //send(server, stop_msg, strlen(stop_msg), 0);
  }
}

int main (int argc, char *argv[])
{
  int s;
  struct sockaddr_in addr;

  addr.sin_addr.s_addr = inet_addr("78.47.146.248");    /* z.B. inet_addr("127.0.0.1"); */
  addr.sin_port = htons(serverport);             /* z.B. htons(80);              */
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

  /* Disable the Nagle (TCP No Delay) algorithm */
  int flag = 1;
  int ret;
  ret = setsockopt( s, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag) );
  if (ret == -1) {
  printf("Couldn't setsockopt(TCP_NODELAY)\n");
  exit( EXIT_FAILURE );
} 

  ping_back(s);


  close();

  return 0;
}

