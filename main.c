#include"tiny.h"
#include<signal.h>
#include<errno.h>
#include<sys/wait.h>
#include<pthread.h>

#define INVALID_SOCKET ~0

extern void doit(int fd);

void handler(int sig)
{
    int olderrno = errno;
    while(waitpid(-1, NULL, 0) > 0)
    {
        printf("Handler reaped child\n");
    }
    if(errno != ECHILD)
    {
        printf("wait error");
    }
    sleep(1);
    errno = olderrno;
}

void *thread(void *vargp)
{
    printf("tetetstatstatasetasast\n");
    int connfd = *((int*)vargp);
    pthread_detach(pthread_self());
    free(vargp);
    doit(connfd);
    close(connfd);
    return NULL;
}
int main(int argc, char**argv){
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    int listenfd, *connfd;
    pthread_t do_t;
    char hostname[MAXLINE], portname[MAXLINE];

    if(argc!=2){
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    
    signal(SIGCHLD, handler);

    listenfd = open_listenfd(argv[1]);

    while(listenfd == -1)
    {
        printf("invalid port, Please reinput:\n");
        scanf("%s", argv[1]);
        listenfd = open_listenfd(argv[1]);
    }

    while(1){
        clientlen = sizeof(clientaddr);
        connfd = malloc(sizeof(int));
        *connfd = accept(listenfd, (SA*)&clientaddr, &clientlen);
        if(*connfd != INVALID_SOCKET)
        {
            getnameinfo((SA*)&clientaddr, clientlen, hostname, MAXLINE, portname, MAXLINE, 0);
            printf("Accept connection from (%s, %s)\n", hostname, portname);
            pthread_create(&do_t, NULL, thread, connfd);
        }
    }
}
