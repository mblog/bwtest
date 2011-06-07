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
  //if(send_bwtest(server) != 0)
  //	return 1;

  return 0;
}

int recv_bwtest(int server)
{
  char buffer[BUFFER_SIZE];
  int bytes, recv_bytes;
  time_t start;
  fd_set rfds;
  struct timeval tv;
  int retval;

  fcntl(server, F_SETFL, O_NONBLOCK);
  
  /* Wait up to five seconds. */
  tv.tv_sec = 5;
  tv.tv_usec = 0;

  start = time(NULL);
  recv_bytes = 0;

  //retval = select(1, &rfds, NULL, NULL, &tv); /* Don't rely on the value of tv now! */

  //if (retval)
  //  printf("Data is available now.\n"); /* FD_ISSET(0, &rfds) will be true. */
  //else
  //  printf("No data within five seconds.\n"); 


  while(1)
  //while (time(NULL)-start <= 5)
  {
    FD_SET(server, &rfds);

    select(server+1, &rfds, NULL, NULL, &tv);
    
    /* if (send(server, request, 1, 0) == -1)
    {
        perror("send() failed");
        return 1;
    } */
    bytes = recv(server, buffer, sizeof(buffer), 0);
    recv_bytes += bytes;
  }

  /*if (send(server, "0", 1, 0) == -1)
    {
        perror("send() failed");
        return 1;
    } */

  printf("Bandbreite: %d\n", (recv_bytes/(time(NULL)-start))*8);
  return 0;

}

int send_bwtest (int server)
{
  char buffer[BUFFER_SIZE];
  time_t start;
  int recv_bytes;

  start = time(NULL);

  // Create Bufer
  while(strlen(buffer) < BUFFER_SIZE-5)
  {
    strcat(buffer, "bw");
  }

  char test[] = "TEST";
  while (time(NULL)-start <= 5)
  {
    send(server, buffer, strlen(buffer),0);
    //send(server, test, strlen(test),0);
  }
 printf ("%d\n", strlen(buffer));

 recv(server, buffer, 1024, 0);

 printf ("Der Server hat %s empfangen\n", buffer);
  return 0;
}

/* ------------------------------ */

int main (int argc, char *argv[])
{
  int s;
  struct sockaddr_in addr;

  addr.sin_addr.s_addr = inet_addr("78.47.146.248");    /* z.B. inet_addr("127.0.0.1"); */
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

  close(s);

  return 0;
}

