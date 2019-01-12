/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE     256

void error(const char *message){
    perror(message);
    exit(1);
}

void* handle_data(void * NewSocketFileDescriptor){
    int SocketFileDescriptor = *((int *) NewSocketFileDescriptor);
    int Result;
    char Buffer[BUFFER_SIZE];

    // Read data from client
    while(1)
    {
	    bzero(Buffer, BUFFER_SIZE);
	    Result = read(SocketFileDescriptor, Buffer, BUFFER_SIZE-1);
	    if(0 > Result) {
	         error("Error reading from socket");
	    }
	    
	    printf("Here's the message: %s\n", Buffer);
	    Result = write(SocketFileDescriptor, "I got your message", 18);
            if(0 > Result) {
		 error("Error writing to socket");
	    }

    }

    close(SocketFileDescriptor);
    // exit the thread
    pthread_exit(NULL);
}


int main(int argc, char *argv[]){
    int SocketFileDescriptor, NewSocketFileDescriptor, PortNumber;
    socklen_t ClientLength;
    struct sockaddr_in ServerAddress, ClientAddress;
    
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
    printf("Started listening\n");
    while(1)
    {
	    ClientLength = sizeof(ClientAddress);
	    // Accept connection from client
	    
	    NewSocketFileDescriptor = accept(SocketFileDescriptor, (struct sockaddr *)&ClientAddress, &ClientLength);

	    if(0 > NewSocketFileDescriptor){ 
		error("ERROR on accept");
	    }
            
            printf("A new client has joined\n");

	    // Multithreading
	    pthread_t thread_id;
	    if(pthread_create(&thread_id, NULL, &handle_data, (void *)(&NewSocketFileDescriptor)) == -1) {
                error("pthread create error");
            }
    }
    close(SocketFileDescriptor);
    return 0; 
}
