/* A simple server in the internet domain using TCP
   The port number is passed as an argument */

/* Updated version to support multi-client connections using
   epoll */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <poll.h>
#include <sys/epoll.h>
#include <vector>
#include <string>
#include <iostream>
#include <signal.h> //  our new library

#define BUF_SIZE 200
#define MAX_CONNECTION 10

using namespace std;

struct ClientDescriptor{
    int FD;
    string username;
};

int glbl_sockFD;

void End_Program(int sig){	//Particularly, when we get ctrl+c signal (SIGINT), we will close FD before exit.

    fflush(stdout);
    printf("\nClosing Socket FD: %d\n",glbl_sockFD);
    close(glbl_sockFD);
    exit(0);
}

int main(int argc, char *argv[])
{

    int ServerSocketFD, ClientSocketFD;
    socklen_t ClientLength;
    struct sockaddr_in ServerAddress, ClientAddress;
    char buff[BUF_SIZE];
    int ret;
    ClientDescriptor UserInfoCD;
    vector<ClientDescriptor> ClientList;

    if(2 > argc){
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    int portNumber = atoi(argv[1]);
    if((1 > portNumber)||(65535 < portNumber)){
        fprintf(stderr,"Port %d is an invalid port number\n",portNumber);
        exit(0);
    }

    // Creating socket
    if ((ServerSocketFD = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr,"Socket error:%s\n\a", strerror(errno));
        exit(1);
    }

    glbl_sockFD = ServerSocketFD;
    signal(SIGINT, End_Program); //we want to handle the ctrl+c command, will trigger closing of port then exit program.
  
    // Setup ServerAddress data structure
    bzero((char *) &ServerAddress, sizeof(ServerAddress));
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    ServerAddress.sin_port = htons(portNumber);
  
    // Binding socket to port
    if (bind(ServerSocketFD, (struct sockaddr *)(&ServerAddress), sizeof(ServerAddress)) == -1) {
        fprintf(stderr,"Bind error:%s\n\a", strerror(errno));
        exit(1);
    }
    // Listing for client
    if(listen(ServerSocketFD, 5) == -1) {
        fprintf(stderr,"Listen error:%s\n\a", strerror(errno));
        exit(1);
    }
    int i;
    int timeout = 5000; // time out value
    struct epoll_event eventList[MAX_CONNECTION]; //event list

    // Creating and initializing epoll
    int epollfd = epoll_create(MAX_CONNECTION);
    
    // Add sockt to epoll for listening
    struct epoll_event event;
    event.events = EPOLLIN|EPOLLET;
    event.data.fd = ServerSocketFD;
    if(epoll_ctl(epollfd, EPOLL_CTL_ADD, ServerSocketFD, &event) < 0) {
        printf("epoll add fail : fd = %d\n", ServerSocketFD);
        exit(1);
    }
    while(1)
    {
        // print out client list
        printf("Connected Client List:\n");
        for(unsigned int k = 0; k < ClientList.size(); k++){
            cout << ClientList[k].username << endl;
        }
        // get active number of connections from epoll
        int active_num = epoll_wait(epollfd, eventList, MAX_CONNECTION, -1);
        printf("active_num (Number of I/O requests ready): %d\n", active_num);
        if(active_num < 0) {
            printf("epoll wait error\n");
	    break;
        } 
	else if(active_num == 0) {
	    printf("timeout ...\n");
	    continue;
        }
        // go through active connection
        for(i = 0; i < active_num; i++) {
            // continue if not readable
            if (!(eventList[i].events & EPOLLIN)) {
                printf ( "event: %d\n", eventList[i].events);
	        continue;
	    }
	    // checkc if it is a new connection
	    if (eventList[i].data.fd == ServerSocketFD) {
                ClientLength = sizeof(ClientAddress);
	        ClientSocketFD = accept(ServerSocketFD, (struct sockaddr *)&ClientAddress, &ClientLength);
                if (ClientSocketFD < 0) {
                    printf("accept error\n");
		    continue;
	        }
	        UserInfoCD.FD = ClientSocketFD;
	        // Add new connection to epoll for listening
	        struct epoll_event event;
	        event.data.fd = ClientSocketFD;
	        event.events =  EPOLLIN|EPOLLET;
	        epoll_ctl(epollfd, EPOLL_CTL_ADD, ClientSocketFD, &event);
	    } 
	    else {
  	        // accepting data
                ret = recv(eventList[i].data.fd, buff, BUF_SIZE, 0);
	        if (ret <= 0) {
                // client closed
	        printf("client[%d] closed\n", eventList[i].data.fd);
		close(eventList[i].data.fd);
	        printf("Which File Descriptor: %d\n", eventList[i].data.fd);

	        for(int n = 0; n < ClientList.size(); n++){
          	    if(ClientList[n].FD == eventList[i].data.fd){
		        ClientList.erase(ClientList.begin()+n); //Client leaving, remove them from the users list!
	            }
                }
  
		// delete the connection from epoll
	        epoll_ctl(epollfd, EPOLL_CTL_DEL, eventList[i].data.fd, NULL);
           	}
		else {
                    // add end character
		    if (ret < BUF_SIZE) {
		        memset(&buff[ret - 1], '\0', 1);
            	    }
            	    printf("client[%d] send:%s\n", eventList[i].data.fd, buff);

		    // add current connection and copy the username to client list
	            UserInfoCD.username = string(buff);
            	    ClientList.push_back(UserInfoCD);
		    printf("Accept Connection: %d\n", ClientSocketFD);
		      
		    // send message back
		    send(eventList[i].data.fd, "You are connected!", 19, 0);
		}
	    }
       	}
    }

    // close connection
    close(epollfd);
    close(ServerSocketFD);
    exit(0);
}
