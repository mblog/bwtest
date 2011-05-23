/*
        bwtest - TCP Socket Bandwith-Test

        Author: Marc Bertram
*/

#include <stdio.h>
#include <string.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 1024

/* --------- prototypes --------- */

/* Run Tests with Server */
int bwtest(int server);

/* Download(Receive) Test */
int recv_bwtest(int server);

/* Upload(send) Test */
int send_bwtest(int server);

/* ------------------------------ */

int bwtest (int server)
{
  // Download-Test
  printf ("Download\n");
  if(recv_bwtest(server) != 0)
	return 1;

  // Upload-Test
  printf ("Upload\n");
  if(send_bwtest(server) != 0)
	return 1;

  return 0;
}

int recv_bwtest(int server)
{
  char buffer[BUFFER_SIZE];
  char request[] = "1";
  int bytes, x, recv_bytes;
  time_t start;

  start = time(NULL);
  recv_bytes = 0;

  while (time(NULL)-start <= 5)
  {
    if (send(server, request, 1, 0) == -1)
    {
        perror("send() failed");
        return 1;
    }
    bytes = recv(server, buffer, sizeof(buffer), 0);
     if (bytes == -1)
       return 1;
    buffer[bytes] = '\0';

    recv_bytes += bytes;
  }

  if (send(server, "1", 1, 0) == -1)
    {
        printf ("STOP");
        perror("send() failed");
        return 1;
    }

  printf("Bandbreite: %d\n", (recv_bytes/5)*8);
  return 0;

}

int send_bwtest (int server)
{
  return 0;
}

/* ------------------------------ */

int main (int argc, char *argv[])
{
  int s;
  struct sockaddr_in addr;

  addr.sin_addr.s_addr = inet_addr("127.0.0.1");    /* z.B. inet_addr("127.0.0.1"); */
  addr.sin_port = htons(5001);             /* z.B. htons(80);              */
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

  // ToDo
  bwtest(s);

  close();

  return 0;
}

