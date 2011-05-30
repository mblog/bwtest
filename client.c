/*
        bwtest - TCP Socket Bandwith-Test

        Author: Marc Bertram
*/

#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

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

  for (x=1; x<=24; x++)
  {
    for (y = 0; y < 2; y++)
    {
      //send(server, control_msg, strlen(control_msg), 0);
      bytes = recv(server, recv_msg, sizeof(recv_msg), 0);
      recv_msg[bytes] = '\0';
      send(server, recv_msg, x*100, 0);
    }
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

  ping_back(s);


  close();

  return 0;
}

