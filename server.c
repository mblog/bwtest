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


/* ------------------------------ */

int client_behandlung(int client)
{
  char buffer[BUFFER_SIZE];
  char request[10];
  int bytes, x;

  for(x = 0; x < BUFFER_SIZE-10; x++)
  {
    //strcpy(buffer, "Q");
    buffer[x] = 'Q'; 
  }

  printf ("%d\n", strlen(buffer));
  while((bytes = recv(client, request, sizeof(request), 0)) > 0)
  {
    send(client, buffer, strlen(buffer), 0);
  }
  if (bytes == -1)
    return -1;
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
  for(;;)
  {
    cli_size = sizeof(cli);
    c = accept(s, (struct sockaddr *)&cli, &cli_size);
    if (c == -1)
      {
	perror("accept() failed");
	return 5;
      }
    printf("client from %s\n", inet_ntoa(cli.sin_addr));
    // printf("Verbindung von %s\n", inet_ntoa(cli.sin_addr));
    if (client_behandlung(c) == -1)
	fprintf(stderr, "%s: client_behandlung() failed", argv[0]);
    close(c);
  }

  close();

  return 0;
}

