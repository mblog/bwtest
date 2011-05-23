/*
        bwtest - TCP Socket Bandwith-Test

        Author: Marc Bertram
*/

#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 1024

/* --------- prototypes --------- */

// Todo if client connects
int client_behandlung(int client);

// Send Test
int send_bwtest(int client);

// Recv Test
int recv_bwtest (int client);

/* ------------------------------ */

int client_behandlung(int client)
{
  // First a Download-Test (from client view)
  if (send_bwtest(client) != 0)
	return 1;

  // Upload-Test (from client view)
  if (recv_bwtest(client) != 0)
	return 1;

  return 0;
}

int send_bwtest (int client)
{
  char buffer[BUFFER_SIZE];
  char request[10];
  int bytes, x;

  // Create Buffer
  for(x = 0; x < (BUFFER_SIZE-4); x++)
  {
    buffer[x] = 'Q';
  }

  printf("Buffer-Size: %d\n", strlen(buffer));

  // Send buffer if request received
  while((bytes = recv(client, request, sizeof(request), 0)) > 0)
  {
    send(client, buffer, strlen(buffer), 0);
  }
  if (bytes == -1)
    return 1;
  return 0;
}

int recv_bwtest (int client)
{
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

