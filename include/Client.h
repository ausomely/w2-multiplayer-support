#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
//Susing namespace std;

#define BUFFER_SIZE     256

class Client {
  public:
    //protected:
      int SocketFileDescriptor;
      int PortNumber;
      int Result;
      struct sockaddr_in ServerAddress;
      struct hostent *Server;
      char Buffer[BUFFER_SIZE];

    //public:
      void NetworkError(const char *message);
      void Connect(std::string hostName, int portNumber);
      void SendMessage(std::string data);
      void CloseConnection();

};
#endif
