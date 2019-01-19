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

#define BUFFER_SIZE     256

class Client {
    public:
    // members
        int SocketFileDescriptor;
        int PortNumber;
        struct sockaddr_in ServerAddress;
        struct hostent *Server;

    // member functions
        Client();
        bool Connect(std::string hostName, int portNumber);
        void SendMessage(std::string data);
        void CloseConnection();

};
#endif
