/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE     256

void error(const char *message){
    perror(message);
    exit(1);
}

int main(int argc, char *argv[]){
    int SocketFileDescriptor, NewSocketFileDescriptor, PortNumber;
    socklen_t ClientLength;
    char Buffer[BUFFER_SIZE];
    struct sockaddr_in ServerAddress, ClientAddress;
    int Result;
    
    if(2 > argc){
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    PortNumber = atoi(argv[1]);
    if((1 > PortNumber)||(65535 < PortNumber)){
        fprintf(stderr,"Port %d is an invalid port number\n",PortNumber);
        exit(0);
    }
    // Create TCP/IP socket
    SocketFileDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(0 > SocketFileDescriptor){
        error("ERROR opening socket");
    }
    // Setup ServerAddress data structure
    bzero((char *) &ServerAddress, sizeof(ServerAddress));
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_addr.s_addr = INADDR_ANY;
    ServerAddress.sin_port = htons(PortNumber);
    // Binding socket to port
    if(0 > bind(SocketFileDescriptor, (struct sockaddr *)&ServerAddress, sizeof(ServerAddress))){ 
        error("ERROR on binding");
    }
    // Listing for client
    listen(SocketFileDescriptor, 5);
    while(1)
    {
	    ClientLength = sizeof(ClientAddress);
	    // Accept connection from client
	    printf("Listening\n");
	    
	    NewSocketFileDescriptor = accept(SocketFileDescriptor, (struct sockaddr *)&ClientAddress, &ClientLength);

            printf("%d\n", ClientAddress.sin_addr.s_addr);
	    if(0 > NewSocketFileDescriptor){ 
		error("ERROR on accept");
	    }

	    printf("Accepted\n");
	    bzero(Buffer, BUFFER_SIZE);
	    // Read data from client
	    /*
            Result = read(NewSocketFileDescriptor, Buffer, BUFFER_SIZE-1);
	    if(0 > Result){
		error("ERROR reading from socket");
	    }

	    printf("Here is the message: %s\n", Buffer);*/
	    // Write data to client
	    Result = write(NewSocketFileDescriptor, "Connected", 20);
	    if(0 > Result){
		error("ERROR writing to socket");
	    }
            //close(SocketFileDescriptor);
	//    close(NewSocketFileDescriptor);
    }
    close(NewSocketFileDescriptor);
    close(SocketFileDescriptor);
    return 0; 
}
