

#include "config.h"

ListNode *userList = NULL;

int main(void){
    int i, n, ret, maxi, maxfd, listenfd, connfd, sockfd;

    socklen_t clilen;

    pthread_t pid;

    int opt =1;
    struct sockaddr_in servaddr, cliaddr;
    
    fd_set rset, allser;
    int nready, client_sockfd[FD_SETSIZE];

    Message message;

    char buf[MAX_LINE];


    User user;
    /*1*/
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("");
        exit(1);
    }//if

    /*2*/
    bzero(&servaddr, sizeof servaddr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    /*3*/
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
    if(bind(listenfd, (struct sockaddr *)&servaddr, sizeof servaddr)){
        perror("");
        exit(1);
    }

    /*4*/
    if(listen(listenfd, LISTENEQ) < 0){
        perror("");
        exit(1);
    }

    /*5*/
    maxfd = listenfd;
    maxi = -1;
    for(i=0; i<FD_SETSIZE; ++i) client_sockfd[i];

    FD_ZERO(&allser);

    FD_SET(listenfd, &allser);

    while(1){
        rset = allser;

        nready = select(maxfd+1, &rset, NULL, NULL, 0);

        if(FD_ISSET(listenfd, &rset)){
            clilen = sizeof cliaddr;
            if((connfd = accept(listenfd , (struct sockaddr *)&cliaddr , &clilen)) < 0){
				perror("accept error.\n");
				exit(1);
			}//if

            printf("server: got connection from %s\n", inet_ntoa(cliaddr.sin_addr));

            for(i=0; i<FD_SETSIZE; ++i){
                if(client_sockfd[i]<0){
                    client_sockfd[i] = connfd;
                    break;
                }
            }
            if(i == FD_SETSIZE) perror("");

            FD_SET(connfd, &allser);

            if(connfd>maxfd) maxfd = connfd;
            if(i>maxi) maxi = i;
            if(--nready <= 0) continue;
        }
        for(i=0; i<= maxi; ++i){
            if(sockfd=client_sockfd[i] < 0) continue;
            if(FD_ISSET(sockfd, &rset)){
                if(--nready < 0) break;
                pthread_create(&pid, NULL, (void *)handleRequest, (void *)&sockfd);
            }
            FD_CLR(sockfd, &allser);
            client_sockfd[i] = -1;
        }
    }
    close(listenfd);
    return 0;
}

void *handleRequest(int *fd){
    int sockfd, ret, n;

    Message message;

    char buf[MAX_LINE];

    sockfd = *fd;

    memset(buf, 0, MAX_LINE);
    memset(&message, 0, sizeof message);

    n=recv(sockfd, buf, sizeof(buf)+1, 0);
    if(n <= 0){
        fflush(stdout);
        close(sockfd);
        *fd = -1;
        printf("%s\n", inet_ntoa(message.sendAddr.sin_addr));
        return NULL;
    }
    else{
        memcpy(&message, buf, sizeof buf);

        switch(message.msgType){
            case REGISTER:{
                printf("");
                ret = registerUser(&message, sockfd);
                memset(&message,0,sizeof message);
                message.msgType = RESULT;
                message.msgRet = ret;
                strcpy(message.content, stateMsg(ret));
                memset(buf, 0, MAX_LINE);
                memcpy(buf, &message, sizeof message);
                send(sockfd, buf, sizeof buf, 0);
                printf("");
                close(sockfd);
                *fd = -1;
                return NULL;
                break;
            }
            case LOGIN:{
                printf("");
                ret = loginUser(&message, sockfd);
                memset(&message, 0, sizeof message);
                message.msgType = RESULT;
                message.msgRet = ret;
                strcpy(message.content, stateMsg(ret));
                memset(buf, 0, MAX_LINE);
                memcpy(buf,&message, sizeof message);
                send(sockfd, buf, sizeof buf, 0);
                printf("");

                enterChat(&sockfd);
                break;
            }
            default:{
                printf("");
                break;
            }
        }
    }
    close(sockfd);
    *fd = -1;
    return NULL;
}