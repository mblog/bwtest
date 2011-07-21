/*
        bwtest - TCP Socket Bandwith-Test

        Author: Marc Bertram
*/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

#define TEST_TIME 10
#define BUFFER_SIZE 10000
#define VERSION "0.30"

/* --------- prototypes --------- */

// Check Client-Version
int check_client_version(int client);

// Todo if client connects
int client_behandlung(int client);

/* ------------------------------ */

int check_client_version(int client)
{
  char buffer[BUFFER_SIZE];
  int bytes;

  bytes = recv(client, buffer, sizeof(buffer),0);
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
  char buffer[BUFFER_SIZE];
  char msg[8];
  time_t start;
  fd_set rfds;
  int bytes;
  unsigned int *recv_bytes;
  unsigned long int byte_count;
  unsigned int *diff;
  struct timeval timeout;
  struct timeval start_time, end_time;
  int first_interval = 1;

  // Receive
  printf ("Receive from client\n");
  //fcntl(client, F_SETFL, O_NONBLOCK);

  /* Wait up to five seconds. */
  timeout.tv_sec = 5;
  timeout.tv_usec = 0;

  byte_count = 0;
  FD_ZERO(&rfds);

  while(1)
  {
    //FD_ZERO(&rfds);
    FD_SET(client, &rfds);

    // Check Sockets
    select(client+1, &rfds, NULL, NULL, &timeout);

    if (FD_ISSET(client, &rfds))
    {
      // On First Test-Interval set Start-Time
      if (first_interval == 1)
      {
        gettimeofday(&start_time, NULL);
        first_interval = 0;
        printf ("Data\n");
      }
      timeout.tv_sec = 1;
      timeout.tv_usec = 0;
      bytes = recv(client, buffer, sizeof(buffer), 0);
      byte_count += bytes;
    }
    else
    {
      printf("No more data\n");
      gettimeofday(&end_time, NULL);
      break;
    }
  }

  diff = (unsigned int*) (msg );
  recv_bytes = (unsigned int *) (msg +4 );
  memset(msg, (unsigned char) 0x00, 8);

  *diff = (end_time.tv_sec*1000+end_time.tv_usec/1000)-(start_time.tv_sec*1000+start_time.tv_usec/1000)-1000;
  *recv_bytes = byte_count;
  printf ("Send data to client\n");
  printf ("Receive Time: %u\n", *diff);
  printf ("Received Bytes: %u\n", *recv_bytes);
  send(client, msg, 8,0);

   // Send
  printf ("Send to Client\n");
  // Set Start-Time and Send to Client
  start = time(NULL);
  while ((time(NULL)-start) < (*diff+500)/1000)
  {
     if (send(client, buffer, BUFFER_SIZE, MSG_NOSIGNAL) < 0)
     {
	perror("Send error\n");
	return -1;
     }
  }

  return 0;
}

/* ------------------------------ */

int main (int argc, char *argv[])
{
  int s, c, flag, ret;
  int sock_buf_size;
  struct sockaddr_in addr;
  struct sockaddr_in cli;
  int cli_size;

  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(40001);
  addr.sin_family = AF_INET;

  // Create Socket
  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s == -1)
  {
    perror("socket() failed");
    return 1;
  }

  /* Disable the Nagle (TCP No Delay) algorithm  */
  /* flag = 1;
  ret = setsockopt( s, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag) );
  if (ret == -1) {
    printf("Couldn't setsockopt(TCP_NODELAY)\n");
    exit( EXIT_FAILURE );
  } */

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

  sock_buf_size = 1000000;
  if (setsockopt( s, SOL_SOCKET, SO_SNDBUF,(void *)&sock_buf_size, sizeof(sock_buf_size) ) < 0) {
        perror ("setsocketopt() failed");
  }
  if (setsockopt( s, SOL_SOCKET, SO_RCVBUF,(void *)&sock_buf_size, sizeof(sock_buf_size) ) < 0) {
        perror ("setsocketopt() failed");
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
	continue;
      }
    printf("Client from %s\n", inet_ntoa(cli.sin_addr));
    /*if (check_client_version(c) == -1)
      {
        perror("Falsche Client-Version\n"); 
        continue;
      } */

    /* enable non-blocking operation */
      /*  if (fcntl(c, F_SETFL, O_NONBLOCK) < 0) {
		//printf("-ERR Can't set non-blocking operation: %s\r\n", strerror(errno));
		close(c);
		return 1;
	} */

    if (client_behandlung(c) == -1)
	perror("client_behandlung() failed\n");
    close(c);
  }

  close(s);

  return 0;
}

