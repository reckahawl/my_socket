
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>

const int MAX_LINE = 2048;
const int PORT = 6001;
const int BACKLOG = 10;
const int LISTENQ = 6666;
const int MAX_CONNECT = 20;



void *recv_message(void *fd)
{
	int sockfd = *(int *)fd;
	while(1)
	{
		char buf[MAX_LINE];
		memset(buf , 0 , MAX_LINE);
		int n;
		if((n = recv(sockfd , buf , MAX_LINE , 0)) == -1)
		{
			perror("recv error.\n");
			exit(1);
		}//if
		buf[n] = '\0';		
		
		if(strcmp(buf , "byebye.") == 0)
		{
			printf("Client closed.\n");
			close(sockfd);
			exit(1);
		}//if

		printf("\nClient: %s\n", buf);
	}//while
}

int main()
{


	int listenfd , connfd;
	socklen_t clilen;
	
	pthread_t recv_tid , send_tid;

	
	struct sockaddr_in servaddr , cliaddr;
	
	
	if((listenfd = socket(AF_INET , SOCK_STREAM , 0)) == -1)
	{
		perror("socket error.\n");
		exit(1);
	}//if

	
	bzero(&servaddr , sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	
	if(bind(listenfd , (struct sockaddr *)&servaddr , sizeof(servaddr)) < 0)
	{
		perror("bind error.\n");
		exit(1);
	}//if

	
	if(listen(listenfd , LISTENQ) < 0)
	{
		perror("listen error.\n");
		exit(1);
	}//if

	

	clilen = sizeof(cliaddr);
	if((connfd = accept(listenfd , (struct sockaddr *)&cliaddr , &clilen)) < 0)
	{
		perror("accept error.\n");
		exit(1);
	}//if

	printf("server: got connection from %s\n", inet_ntoa(cliaddr.sin_addr));

	
	if(pthread_create(&recv_tid , NULL , recv_message, &connfd) == -1)
	{
		perror("pthread create error.\n");
		exit(1);
	}//if

	char msg[MAX_LINE];
	memset(msg , 0 , MAX_LINE);
	while(fgets(msg , MAX_LINE , stdin) != NULL)	
	{	
		if(strcmp(msg , "exit\n") == 0)
		{
			printf("byebye.\n");
			memset(msg , 0 , MAX_LINE);
			strcpy(msg , "byebye.");
			send(connfd , msg , strlen(msg) , 0);
			close(connfd);
			exit(0);
		}//if

		if(send(connfd , msg , strlen(msg) , 0) == -1)
		{
			perror("send error.\n");
			exit(1);
		}//if		
	}//while
}


