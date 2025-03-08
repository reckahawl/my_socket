#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>


#ifndef CONNECT_SIZE
#define CONNECT_SIZE 256
#endif

#define PORT 7777
#define MAX_LINE 2048
#define LISTENQ 20

void setNonblocking(int sockfd){
	int opts;
	opts = fcntl(sockfd, F_GETFL);
	if(opts<0){
		perror();
		return;
	}
	opts = opts | O_NONBLOCK;
	if(fcntl(sockfd, F_SETFL,opts) < 0){
		perror();
		return;
	}
}

int main(int argc, char **argv){
	int i, listenfd, connfd, sockfd, epfd, nfds;
	ssize_t n, ret;

	char buf[MAX_LINE];
	socklen_t clilen;

	struct sockaddr_in servaddr, cliaddr;

	struct epoll_event ev, events[20];

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	setNonblocking(listenfd);

	epfd = epoll_create(CONNECT_SIZE);
	ev.data.fd = listenfd;
	ev.events = EPOLLIN | EPOLLET;
	epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	listen(listenfd, LISTENQ);

	while(1){
		nfds = epoll_wait(epfd, events, CONNECT_SIZE, -1);
		if(nfds <= 0) continue;
		printf("nfds = %d\n", nfds);

		for(i=0; i<nfds; ++i){
			if(events[i].data.fd == listenfd){
				clilen = sizeof(cliaddr);
				if((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen)) < 0){
					perror();
					exit(1);
				}
				printf("accept a new client: %s:%d\n", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);

				setNonblocking(connfd);
				ev.data.fd = connfd;
				ev.events = EPOLLIN | EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
			}else if(events[i].events & EPOLLIN){
				if((sockfd = events[i].data.fd)<0) continue;
				bzero(buf, MAX_LINE);
				printf("reading the socket");
				if((n=read(sockfd, buf, MAX_LINE)) <=0 ){
					close(sockfd);
					events[i].data.fd = -1;
				}else{
					buf[m] = '\0';
					printf("clint[%d] send message: %s\n", i , buf);

					ev.data.fd = sockfd;
					ev.events = EPOLLOUT | EPOLLET;
					epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev)
				}
			}else if(events[i].events & EPOLLOUT){
				if((sockfd = events[i].data.fd) < 0) continue;
				if((ret=write(sockfd, buf, n)) != n){
					printf("erroe");
					break;
				}

				ev.data.fd = sockfd;
				ev.events = EPOLLIN | EPOLLET;

				epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
			}
		}
	}
	free(events);
	close(epfd);
	exit(0);
}
