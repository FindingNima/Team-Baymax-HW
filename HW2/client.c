
/*
Showing example of SOCKETS on LOCAL HOST
*/
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

/*
  Write text to socket given by file descriptor SOCKET_FD
*/

void write_text(int socket_fd, const char* text)
{
   //write the Number of Bytes in the String including NULL termination
  int length = strlen(text) + 1;
  write (socket_fd, &length, sizeof (length));

   //Write the actual String
  write(socket_fd, text, length);   
}

int main(int argc, char* const argv[])
{
  const char* const socket_name = argv[1];
  const char* const message = argv[2];
  int socket_fd;     //like a file descriptor used as handle to socket
  struct sockaddr_un name;
  
   //Create Socket - establish phone
  socket_fd = socket (PF_LOCAL, SOCK_STREAM, 0);    //Using Local namespace,SOCK_STREAM-Connection style
  
   // Store server name in socket address -- Receiver in the phone system and get phone #
  name.sun_family = AF_LOCAL;
  strcpy (name.sun_path, socket_name);
  
   //Connect to the server - Call Server
  connect (socket_fd, &name, SUN_LEN (&name));

   //Write text on command line to the socket
  write_text (socket_fd, message);
  
   //Close connection -  HANG UP
  close(socket_fd);
 
  return 0;
}
