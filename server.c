/*
        bwtest - TCP Socket Bandwith-Test

        Author: Marc Bertram
*/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define TEST_TIME 5
#define BUFFER_SIZE 1024
#define VERSION "0.1"

/* --------- prototypes --------- */

// Check Client-Version
int check_client_version(int client);

// Todo if client connects
int client_behandlung(int client);

/* ------------------------------ */

int check_client_version(int client)
{
  char buffer[BUFFER_SIZE];
  double client_version;
  int bytes;

  bytes = recv(client, buffer, sizeof(buffer),0);
  //sprintf(client_version, "%f", buffer); 
  buffer[bytes] = '\0';
  printf ("Client Version: %s\n", buffer);
  printf ("Server Version: %s\n", VERSION);
  send(client, VERSION, strlen(VERSION), 0);
  if (strcmp (buffer, VERSION) != 0)
   return -1;
  return 0;
}

int client_behandlung(int client)
{
  char buffer[BUFFER_SIZE], bbandwidth[20];
  time_t start;
  fd_set rfds;
  int bytes;
  unsigned long long recv_bytes, bandwidth;
  struct timeval tv;
  int retval;
  int first_interval = 1;

  printf ("Send to Client\n");

  start = time(NULL);

  while ((time(NULL)-start) < TEST_TIME)
  {
     if (send(client, buffer, BUFFER_SIZE, MSG_NOSIGNAL) < 0)
     {
	perror("Send error\n");
	return -1;
     }
  }

  printf ("Receive from client\n");

  //fcntl(client, F_SETFL, O_NONBLOCK);

  /* Wait up to five seconds. */
  tv.tv_sec = 5;
  tv.tv_usec = 0;

  recv_bytes = 0;
  //start = time(NULL);

  while(1)
  {
    FD_ZERO(&rfds);
    FD_SET(client, &rfds);

    select(client+1, &rfds, NULL, NULL, &tv);

    // bytes = recv(client, buffer, sizeof(buffer), 0);

    if (FD_ISSET(client, &rfds))
    //if (bytes > 0)
    {
      //printf("Data is available now.\n");
      if (first_interval == 1)
      {
        start = time(NULL);
        first_interval = 0;
        printf ("Data\n");
        //tv.tv_sec = 0;
      }
      tv.tv_sec = 1;
      bytes = recv(client, buffer, sizeof(buffer), 0);
      recv_bytes += bytes;
    }
    else
    {
      printf("No more data\n");
      //printf("Bytes %d\n", recv_bytes);
      bandwidth = (recv_bytes/(time(NULL)-start-1))*8;
      //printf("bandwidth: %d\n", bandwidth);
      //return 0;
      break;
    }
  }

  //printf("Bandbreite: %d\n", (int)(recv_bytes/(time(NULL)-start))*8);

  printf ("Send receive bandwitdth to client\n");

  //itoa(bandwidth ,bbandwidth, 10);
  sprintf(bbandwidth, "%d", bandwidth);
  send(client, bbandwidth, strlen(bbandwidth), 0); 

  //printf ("===============================\n");

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
    printf ("===============================\n");
    cli_size = sizeof(cli);
    c = accept(s, (struct sockaddr *)&cli, &cli_size);
    if (c == -1)
      {
	perror("accept() failed");
	return 4;
      }
    printf("Client from %s\n", inet_ntoa(cli.sin_addr));
    if (check_client_version(c) == -1)
      {
        fprintf(stderr, "Falsche Client-Version\n"); 
        continue;
      }
    if (client_behandlung(c) == -1)
	fprintf(stderr, "%s: client_behandlung() failed\n", argv[0]);
    close(c);

    //printf ("===============================\n");
  }

  close(s);

  return 0;
}

