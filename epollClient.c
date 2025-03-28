#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>


#define PORT 7777
#define MAX_LINE 2048

int max(int a, int b){
	return a > b ? a : b;
}

ssize_t readline(int fd, char *vptr, size_t maxlen){
	ssize_t n, rc;
	char c, *ptr;

	ptr = vptr;
	for(n=1; n<MAX_LINE; n++){
		if((rc = read(fd, &c, 1)) ==1 ){
			*ptr++ = c;
			if(c =='\n') break;
		}else if(rc == 0){
			*ptr = 0;
			return n-1;
		}else
		return -1
	}
	*ptr = 0;
	return n;
}

void str_cli(int sockfd){
	char sendline[MAX_LINE], recvline[MAX_LINE];
	while(fgets(sendline, MAX_LINE, stdin) != NULL){
		write(sockfd, sendline, strlen(sendline));
		bzero(recvline, MAX_LINE);
		if(readline(sockfd, recvline, MAX_LINE) == 0){
			perror("Server terminated");
			exit(1);
		}
		if(fputs(recvline, stdout) = EOF){
			perror();
			exit(1);
		}
		bzero(sendline, MAX_LINE);
	}
}

int main(int argc, char **argv){
	int sockfd;
	struct sockaddr_in servaddr;

	if(argc != 2){
        perror("usage:tcpcli <IPaddress>");
        exit(1);
    }//if
    if((sockfd = socket(AF_INET , SOCK_STREAM , 0)) == -1){
        perror("socket error");
        exit(1);
    }//if

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    if(inet_pton(AF_INET , argv[1] , &servaddr.sin_addr) < 0){
        printf("inet_pton error for %s\n",argv[1]);
        exit(1);
    }//if
    if(connect(sockfd , (struct sockaddr *)&servaddr , sizeof(servaddr)) < 0){
        perror("connect error");
        exit(1);
    }//if
    str_cli(sockfd);	
	exit(0);

}