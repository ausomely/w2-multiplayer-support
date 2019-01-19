#include "Client.h"

Client::Client() {}

bool Client::Connect(std::string hostName, int portNumber){
    PortNumber = portNumber;
    if((1 > PortNumber)||(65535 < PortNumber)){
        fprintf(stderr,"Port %d is an invalid port number, try another port\n",PortNumber);
        return false;
    }
    // CreateNetworkError TCP/IP socket
    printf("Creating socket\n");
    SocketFileDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(0 > SocketFileDescriptor){
        perror("ERROR opening socket");
        return false;
    }
    // Convert/resolve host name
    Server = gethostbyname(hostName.c_str());
    if(NULL == Server){
        fprintf(stderr,"ERROR, no such host\n");
        return false;
    }
    // Setup ServerAddress data structure
    bzero((char *) &ServerAddress, sizeof(ServerAddress));
    ServerAddress.sin_family = AF_INET;
    bcopy((char *)Server->h_addr, (char *)&ServerAddress.sin_addr.s_addr, Server->h_length);
    ServerAddress.sin_port = htons(PortNumber);
    // Connect to server
    if(0 > connect(SocketFileDescriptor, (struct sockaddr *)&ServerAddress, sizeof(ServerAddress))){
        perror("ERROR connecting");
        return false;
    }
    return true;
}

void Client::SendMessage(std::string data){
      int Result;
      // Write data to server
      Result = write(SocketFileDescriptor, data.c_str(), strlen(data.c_str()));
      if(0 > Result){
           perror("ERROR writing to socket");
      }

      char Buffer[BUFFER_SIZE];
      bzero(Buffer, BUFFER_SIZE);
      // Read data from server
      Result = read(SocketFileDescriptor, Buffer, BUFFER_SIZE-1);
      if(0 > Result){
          perror("ERROR reading from socket");
      }
      printf("%s\n",Buffer);
}

void Client::CloseConnection(){
    close(SocketFileDescriptor);
}
