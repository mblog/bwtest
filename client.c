/*
        bwtest - TCP Socket Bandwith-Test

        Author: Marc Bertram
*/

#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 1024

/* ------------------------------ */

int server_behandlung (int server)
{
  char buffer[BUFFER_SIZE];
  char request[] = "request";
  int bytes, x;

  for (x = 0; x < 2; x++)
  {
    if (send(server, request, sizeof(request), 0) == -1)
    {
        printf("Feler");
        perror("send() failed");
        return 1;
    }

    bytes = recv(server, buffer, sizeof(buffer), 0);
     if (bytes == -1)
       return -1;
    buffer[bytes] = '\0';

    printf("%s\n", buffer);
    printf("%d Bytes\n", bytes);
  }

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

  server_behandlung(s);   


  close();

  return 0;
}

