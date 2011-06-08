/*
        bwtest - TCP Socket Bandwith-Test

        Author: Marc Bertram
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

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

  close();

  return 0;
}

