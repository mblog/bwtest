/*
        bwtest - TCP Socket Bandwith-Test

        Author: Marc Bertram
*/

#include <sys/types.h>
#include <sys/socket.h>

/* ------------------------------ */


int main (int argc, char *argv[])
{

  // Create Socket
  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s == -1)
  {
    perror("socket() failed");
    return 1;
  }

  

  close();

  return 0;
}

