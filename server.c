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

//char* sock_buf_size;

/* --------- prototypes --------- */

// Check Client-Version
int check_client_version(int client);

// Get Socket-Buffer-Size from client
int get_socket_buffersize(int client);

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

/*int get_socket_buffersize(int client)
{
  char buffer[BUFFER_SIZE];
  int bytes;

  bytes = recv(client, buffer, sizeof(buffer),0);
  buffer[bytes] = '\0';
  return atoi(buffer);
}*/

int client_behandlung(int client)
{
  char buffer[BUFFER_SIZE];
  char msg[1024];
  time_t start;
  fd_set rfds;
  int bytes;
  unsigned long int *recv_bytes;
  unsigned long int byte_count;
  //unsigned long long bandwidth;
  unsigned int *diff;
  struct timeval timeout;
  struct timeval start_time, end_time;
  int first_interval = 1;

  // Send
  printf ("Send to Client\n");
  // Set Start-Time and Send to Client
  start = time(NULL);
  while ((time(NULL)-start) < TEST_TIME)
  {
     if (send(client, buffer, BUFFER_SIZE, MSG_NOSIGNAL) < 0)
     {
	perror("Send error\n");
	return -1;
     }
  }

  // Receive
  printf ("Receive from client\n");
  //fcntl(client, F_SETFL, O_NONBLOCK);

  /* Wait up to five seconds. */
  timeout.tv_sec = 5;
  timeout.tv_usec = 0;

  byte_count = 0;

  while(1)
  {
    FD_ZERO(&rfds);
    FD_SET(client, &rfds);

    // Check Sockets
    select(client+1, &rfds, NULL, NULL, &timeout);

    if (FD_ISSET(client, &rfds))
    {
      // On First Test-Interval set Start-Time
      if (first_interval == 1)
      {
        //start = time(NULL);
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
      //bandwidth = (recv_bytes/((end_time.tv_sec+end_time.tv_usec/1000000.0)-(start_time.tv_sec+start_time.tv_usec/1000000.0)-1))*8;
      break;
    }
  }

  diff = (unsigned int*) (msg + 4);
  recv_bytes = (unsigned long int *) (msg + 8);
  memset(msg, (unsigned char) 0x00, 1024);

  *diff = (end_time.tv_sec*1000+end_time.tv_usec/1000)-(start_time.tv_sec*1000+start_time.tv_usec/1000)-1000;
  *recv_bytes = byte_count;
  printf ("Send data to client\n");
  //sprintf(buffer, "%u:%lu", diff, byte_count);
  printf ("Dauer: %u\n", *diff);
  //send(client, buffer, strlen(buffer), 0);
  //sprintf(buffer, "%lu", recv_bytes);
  printf ("Bytes: %lu\n", *recv_bytes);
  send(client, msg, 1024,0);

  return 0;
}

/* ------------------------------ */

int main (int argc, char *argv[])
{
  int s, c, flag, ret;
  //int sock_buf_size;
  //char bdp[10];
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

    // Get Socket Buffer Size from Client
    //sprintf(bdp, "%d", get_socket_buffersize(c));
    /* sock_buf_size = 1000000;
    printf ("Socket Buffer Size: %d\n", sock_buf_size);
    ret = setsockopt( s, SOL_SOCKET, SO_SNDBUF,(void *)&sock_buf_size, sizeof(sock_buf_size) );
    ret = setsockopt( s, SOL_SOCKET, SO_RCVBUF,(void *)&sock_buf_size, sizeof(sock_buf_size) ); */

    //dup2(c, STDOUT_FILENO);
    //dup2(c, STDIN_FILENO);
    //dup2(c, STDERR_FILENO);

    if (client_behandlung(c) == -1)
	perror("client_behandlung() failed\n");
    close(c);
  }

  close(s);

  return 0;
}

