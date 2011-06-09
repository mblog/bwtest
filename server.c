/*
        bwtest - TCP Socket Bandwith-Test

        Author: Marc Bertram
*/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define TEST_TIME 5
#define BUFFER_SIZE 1024

/* --------- prototypes --------- */

// Todo if client connects
int client_behandlung(int client);

/* ------------------------------ */

int client_behandlung(int client)
{
  char buffer[BUFFER_SIZE];
  time_t start;
  fd_set rfds;
  int bytes, recv_bytes;
  struct timeval tv;
  int retval;


  printf ("Send to Client\n");

  start = time(NULL);

  while ((time(NULL)-start) < TEST_TIME)
  {
     if (send(client, buffer, BUFFER_SIZE, 0) < 0)
     {
	perror("fehlgeschlagen\n");
	return -1;
     }
  }

  printf ("Receive from client\n");

  fcntl(client, F_SETFL, O_NONBLOCK);

  /* Wait up to five seconds. */
  tv.tv_sec = 5;
  tv.tv_usec = 0;

  recv_bytes = 0;
  start = time(NULL);

  while(1)
  {
    FD_ZERO(&rfds);
    FD_SET(client, &rfds);

    select(client+1, &rfds, NULL, NULL, NULL);
    bytes = recv(client, buffer, sizeof(buffer), 0);

    //if (retval)
    if (bytes > 0)
    {
      //printf("Data is available now.\n");
      //bytes = recv(client, buffer, sizeof(buffer), 0);
      recv_bytes += bytes;
    }
    else
    {
      printf("No data\n");
      //printf("Bytes %d\n", recv_bytes);
      printf("Bandbreite: %d\n", (int)(recv_bytes/(time(NULL)-start))*8);
      return 0;
    }
  }
  
  //printf("Bandbreite: %d\n", (int)(recv_bytes/(time(NULL)-start))*8);
  
  return 0;
}

/* ------------------------------ */

int main (int argc, char *argv[])
{
  int s, c;
  struct sockaddr_in addr;
  struct sockaddr_in cli;
  int cli_size;

  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(5001);
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
  for(;;)
  {
    cli_size = sizeof(cli);
    c = accept(s, (struct sockaddr *)&cli, &cli_size);
    if (c == -1)
      {
	perror("accept() failed");
	return 4;
      }
    printf("client from %s\n", inet_ntoa(cli.sin_addr));
    if (client_behandlung(c) == -1)
	fprintf(stderr, "%s: client_behandlung() failed", argv[0]);
    close(c);
  }

  close(s);

  return 0;
}

