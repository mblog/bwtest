/*
        bwtest - TCP Socket Bandwith-Test

        Author: Marc Bertram
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define VERSION "0.1"
#define TEST_TIME 10
#define BUFFER_SIZE 4096

/* --------- prototypes --------- */

/* Send Version to Server */
int version_check (int server);

/* Run Tests with Server */
int bwtest(int server);

/* Download(Receive) Test */
int recv_bwtest(int server);

/* Upload(send) Test */
int send_bwtest(int server);

/* ------------------------------ */

int version_check (int server)
{
  char buffer[] = VERSION;
  int bytes;

  if (send(server, buffer, strlen(buffer), 0) == -1)
    {
      perror ("Senden fehlgeschlagen\n");
      return 1;
    }
  bytes = recv(server, buffer, sizeof(buffer),0);
  buffer[bytes] = '\0';
  if (strcmp(buffer, VERSION) != 0)
    return 1;
  return 0;
}

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
  bytes = recv(server, buffer, sizeof(buffer), 0);
  buffer[bytes] = '\0';
  printf ("Upload Bandbreite:%s\n", buffer);
  return 0;
}

int recv_bwtest(int server)
{
  char buffer[BUFFER_SIZE];
  int bytes;
  unsigned long long recv_bytes;
  //time_t start;
  fd_set rfds;
  struct timeval timeout;
  struct timeval start_time, end_time;
  int first_interval = 1;

  //fcntl(server, F_SETFL, O_NONBLOCK);

  /* Wait up to five seconds. */
  timeout.tv_sec = 5;
  timeout.tv_usec = 0;

  //start = time(NULL);
  recv_bytes = 0;

  while(1)
  {
    FD_ZERO(&rfds);
    FD_SET(server, &rfds);

    // Check Sockets
    select(server+1, &rfds, NULL, NULL, &timeout); /* Don't rely on the value of tv now! */

    if (FD_ISSET(server, &rfds))
    {
      if (first_interval == 1)
      {
        gettimeofday(&start_time, NULL);
        first_interval = 0;
      }
      timeout.tv_sec = 1;
      bytes = recv(server, buffer, sizeof(buffer),0); 
      recv_bytes += bytes;
    }
    else
    {
      gettimeofday(&end_time, NULL);
      printf("Download Bandbreite:%ld\n", recv_bytes/(((end_time.tv_sec*1000+end_time.tv_usec/1000)-(start_time.tv_sec*1000+start_time.tv_usec/1000)-1000)/1000)*8);
      return 0;
    }
  }
  return 0;
}

int send_bwtest (int server)
{
  char buffer[BUFFER_SIZE];
  time_t start;

  start = time(NULL);

  while (time(NULL)-start < TEST_TIME)
  {
    send(server, buffer, BUFFER_SIZE, MSG_NOSIGNAL);
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
            return 1;
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
    return 1;
  }

  // Versions check
  if (version_check(s) == 1)
   {
    perror ("Falsche Client-Version\n");
    return 2;
   }

  // ToDo
  bwtest(s);

  close(s);

  return 0;
}

