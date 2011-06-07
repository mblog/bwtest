/*
        bwtest - TCP Socket Bandwith-Test

        Author: Marc Bertram
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

/* --------- prototypes --------- */

// Todo if client connects
int client_behandlung(int client);

// Receive Request
char receive_request (int client);

// Upload-Test (from client view)
void recv_bwtest (int client);

/* ------------------------------ */

int client_behandlung(int client)
{
  char buffer[BUFFER_SIZE];
  char request;
  int x;
  fd_set rfds;

  printf ("Start Download\n");

  for (x = 0; x< 10;x++)
  {
     FD_SET(client, &rfds);
     select(client+1, &rfds, NULL, NULL, NULL);
     send(client, buffer, strlen(buffer), 0);
  }

  printf ("Start Upload\n");

  // Receive buffer from client (Upload-Test)
  //recv_bwtest(client);

  return 0;
}

/*
void recv_bwtest(int client)
{
 int bytes, recv_bytes;
 char buffer[BUFFER_SIZE]; 
 char str[10];

 printf("%ld\n", sizeof(buffer));

 recv_bytes = 0;
  //while (bytes = recv(client, buffer, sizeof(buffer), 0) > 0)
  do
  {
    bytes = recv(client, buffer, sizeof(buffer), 0);
    buffer[bytes] = '\0';
    recv_bytes += bytes;
  } while (bytes > 0);
 printf ("Empfangene Bytes %d\n", recv_bytes); 
 //itoa (recv_bytes, str, 10); 
 sprintf (buffer, "%d", recv_bytes);
 
 //while((bytes = recv(sock, buffer, sizeof(buffer), 0)) > 0)
        send(client, buffer, strlen(buffer), 0);

 
}
*/

char receive_request(int client)
{
  char recv_message[1];
  int bytes;
  bytes = recv(client, recv_message, 1, 0);
  recv_message[bytes] = '\0';
  return recv_message[0];
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

