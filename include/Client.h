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
using namespace std;

#define BUFFER_SIZE     256

void NetworkError(const char *message);
void Connect(char* hostName, int portNumber);

#endif
