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

#define BUF_SIZE 200

int main(int argc, char *argv[])
{
	int sock_fd, conn_fd;
	struct sockaddr_in server_addr;
	char buff[BUF_SIZE];
	int ret;

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
	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		fprintf(stderr,"Socket error:%s\n\a", strerror(errno));
		exit(1);
	}

	// Setup ServerAddress data structure
	bzero(&server_addr, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(portNumber);

	// Binding socket to port
	if (bind(sock_fd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr)) == -1) {
		fprintf(stderr,"Bind error:%s\n\a", strerror(errno));
		exit(1);
	}

	// Listing for client
	if(listen(sock_fd, 5) == -1) {
		fprintf(stderr,"Listen error:%s\n\a", strerror(errno));
		exit(1);
	}

	int i;
	int conn_num = 5; // allowed maximum connection
	int timeout = 3000; // time out value
	struct epoll_event eventList[conn_num]; //event list

	// Creating and initializing epoll
	int epollfd = epoll_create(conn_num);

	// Add sockt to epoll for listening
	struct epoll_event event;
	event.events = EPOLLIN|EPOLLET;
	event.data.fd = sock_fd;
	if(epoll_ctl(epollfd, EPOLL_CTL_ADD, sock_fd, &event) < 0) {
		printf("epoll add fail : fd = %d\n", sock_fd);
		exit(1);
	}

	while(1) {

		// get active number of connections from epoll
		int active_num = epoll_wait(epollfd, eventList, conn_num, timeout);
		printf("active_num: %d\n", active_num);
		if(active_num < 0) {
			printf("epoll wait error\n");
			break;
		} else if(active_num == 0) {
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
			if (eventList[i].data.fd == sock_fd) {
				conn_fd = accept(sock_fd, (struct sockaddr *)NULL, NULL);

				if (conn_fd < 0) {
					printf("accept error\n");
					continue;
				}
				printf("Accept Connection: %d\n", conn_fd);

				// Add new connection to epoll for listening
				struct epoll_event event;
				event.data.fd = conn_fd;
				event.events =  EPOLLIN|EPOLLET;
				epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_fd, &event);
			} else {

				// accepting data
				ret = recv(eventList[i].data.fd, buff, BUF_SIZE, 0);
				if (ret <= 0) {
					// client closed
					printf("client[%d] close\n", i);
					close(eventList[i].data.fd);

					// delete the connection from epoll
					epoll_ctl(epollfd, EPOLL_CTL_DEL, eventList[i].data.fd, NULL);
				} else {

					// add end character
					if (ret < BUF_SIZE) {
						memset(&buff[ret], '\0', 1);
					}
					printf("client[%d] send:%s\n", i, buff);

					// send message back
					send(eventList[i].data.fd, "Hello", 6, 0);
				}
			}
		}
	}

	// close connection
	close(epollfd);
	close(sock_fd);
	exit(0);
}
