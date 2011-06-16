/*
        bwtest - TCP Socket Bandwith-Test

        Author: Marc Bertram
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#define TEST_TIME 5
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
  char buffer[BUFFER_SIZE];
  //fd_set rfds;
  int bytes;

  // Download-Test
  printf ("Download\n");
  if(recv_bwtest(server) != 0)
	return 1;

  // Upload-Test
  printf ("Upload\n");
  if(send_bwtest(server) != 0)
  	return 1;

  // Get Upload-Bandwidth from server
  printf ("Messung fertig\n");
  //fcntl(server, F_SETFL, O_NONBLOCK);
  //FD_SET(server, &rfds);
  //select(server+1, &rfds, NULL, NULL, NULL);
  //if (FD_ISSET(server, &rfds))
    bytes = recv(server, buffer, sizeof(buffer), 0);
  buffer[bytes] = '\0';
  printf ("Upload Bandbreite: %s\n", buffer);
  return 0;
}

int recv_bwtest(int server)
{
  char buffer[BUFFER_SIZE];
  int bytes, recv_bytes;
  time_t start;
  fd_set rfds;
  struct timeval tv;
  int first_interval = 1;

  //fcntl(server, F_SETFL, O_NONBLOCK);

  /* Wait up to five seconds. */
  tv.tv_sec = 5;
  tv.tv_usec = 0;

  //start = time(NULL);
  recv_bytes = 0;

  while(1)
  {
    FD_ZERO(&rfds);
    FD_SET(server, &rfds);

    //tv.tv_sec = 2;
    select(server+1, &rfds, NULL, NULL, &tv); /* Don't rely on the value of tv now! */
    //bytes = recv(server, buffer, sizeof(buffer), 0);

    //if (bytes > 0)
    if (FD_ISSET(server, &rfds))
    {
      //printf("Data is available now.\n"); /* FD_ISSET(0, &rfds) will be true. */
      if (first_interval == 1)
      {
        start = time(NULL);
        first_interval = 0;
        //tv.tv_sec = 1;
      }
      tv.tv_sec = 1;
      bytes = recv(server, buffer, sizeof(buffer),0); 
      recv_bytes += bytes;
    }
    else
    {
      //printf("No data within five seconds.\n");
      printf("Download Bandbreite: %d\n", (recv_bytes/(time(NULL)-start))*8);
      return 0;
    }
    //printf ("%d", tv.tv_sec);

  }
  //printf("Bandbreite: %d\n", (recv_bytes/(time(NULL)-start))*8);
  return 0;
}

int send_bwtest (int server)
{
  char buffer[BUFFER_SIZE];
  time_t start;

  /*if (send(server, "Upload", strlen("Upload"), MSG_NOSIGNAL) == -1)
    {
      printf ("Senden fehlgeschlagen\n");
      return 0;
    }*/


  start = time(NULL);

  while (time(NULL)-start < TEST_TIME)
  {
    //send(server,buffer, BUFFER_SIZE, 0);
    //printf ("Senden\n");
    if (send(server, buffer, BUFFER_SIZE, MSG_NOSIGNAL) == -1)
    {
      //printf ("Senden fehlgeschlagen\n");
      //return 0;
    }
  }

  return 0;
}

/* ------------------------------ */

int main (int argc, char *argv[])
{
  int s;
  struct sockaddr_in addr;
  struct hostent *host;

  //Check Arguments
  if (argc < 2)
    {
        fprintf(stderr, "usage: %s <host>\n", argv[0]);
        return 1;
    }

    if (!inet_aton(argv[1], &addr.sin_addr))
    {
        host = gethostbyname(argv[1]);
        if (!host)
        {
            herror("gethostbyname() failed");
            return 2;
        }
        addr.sin_addr = *(struct in_addr*)host->h_addr;
    }

  //addr.sin_addr.s_addr = inet_addr();    /* z.B. inet_addr("127.0.0.1"); */
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

