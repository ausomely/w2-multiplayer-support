#include "Client.h"
using namespace std;

void NetworkError(const char *message){
  perror(message);
  exit(0);
}

void Connect(char* hostName, int portNumber){
  int SocketFileDescriptor, PortNumber, Result;
  struct sockaddr_in ServerAddress;
  struct hostent *Server;
  char Buffer[BUFFER_SIZE];

  PortNumber = portNumber;
  if((1 > PortNumber)||(65535 < PortNumber)){
      fprintf(stderr,"Port %d is an invalid port number\n",PortNumber);
      exit(0);
  }
  // CreateNetworkError TCP/IP socket
  printf("Creating socket\n");
  SocketFileDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(0 > SocketFileDescriptor){
      NetworkError("ERROR opening socket");
  }
  // Convert/resolve host name
  Server = gethostbyname(hostName);
  if(NULL == Server){
      fprintf(stderr,"ERROR, no such host\n");
      exit(0);
  }
  // Setup ServerAddress data structure
  bzero((char *) &ServerAddress, sizeof(ServerAddress));
  ServerAddress.sin_family = AF_INET;
  bcopy((char *)Server->h_addr, (char *)&ServerAddress.sin_addr.s_addr, Server->h_length);
  ServerAddress.sin_port = htons(PortNumber);
  // Connect to server
  if(0 > connect(SocketFileDescriptor, (struct sockaddr *)&ServerAddress, sizeof(ServerAddress))){
      NetworkError("ERROR connecting");
  }

  bzero(Buffer, BUFFER_SIZE);
  // Read data from server
  Result = read(SocketFileDescriptor, Buffer, BUFFER_SIZE-1);
  if(0 > Result){
      NetworkError("ERROR reading from socket");
  }
  printf("%s\n",Buffer);
  close(SocketFileDescriptor);
}
