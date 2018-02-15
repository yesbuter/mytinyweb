
#include<errno.h>
#include<string.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>
#include<netdb.h>
#include<stdio.h>
#include<unistd.h>
#define LISTENQ 20
#define RIO_BUFSIZE 8192

typedef struct{
    int rio_fd;
    int rio_cnt;
    char *rio_bufptr;
    char rio_buf[RIO_BUFSIZE];
}rio_t;

int open_clientfd(char *hostname, char *port){
    int rc, clientfd;
    struct addrinfo hints, *listp, *p;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype=SOCK_STREAM;
    hints.ai_flags=AI_NUMERICSERV | AI_ADDRCONFIG;
    if((rc = getaddrinfo(hostname, port, &hints, &listp))!=0){
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(rc));
        exit(1);
    }
    
    for(p = listp; p; p = p->ai_next){
        if((clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))<0)
            continue;
        if(connect(clientfd, p->ai_addr, p->ai_addrlen) != -1)
            break;
        close(clientfd);
    }
    freeaddrinfo(listp);
    return p ? clientfd : -1;
}

int open_listenfd(char *port){
    struct addrinfo *listp, hints, *p;
    int listenfd, optval = 1;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE|AI_NUMERICSERV;
    hints.ai_flags|=AI_ADDRCONFIG;
    getaddrinfo(NULL, port, &hints, &listp);

    for(p=listp; p; p=p->ai_next){
        if((listenfd=socket(p->ai_family, p->ai_socktype, p->ai_protocol))<0)
            continue;

        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval, sizeof(int));

        if(bind(listenfd, p->ai_addr, p->ai_addrlen)==0)
            break;
        close(listenfd);
    }
    freeaddrinfo(listp);
    if(!p)
        return -1;
    if(listen(listenfd, LISTENQ)<0){
        close(listenfd);
        return -1;
    }
    return listenfd;
    
}




void rio_readinitb(rio_t *rp, int fd){
    rp->rio_fd = fd;
    rp->rio_cnt = 0;
    rp->rio_bufptr = rp->rio_buf;
}

ssize_t rio_writen(int fd, void *userbuf, size_t n){
     size_t nleft = n;
     ssize_t nwritten;
     char *bufp = userbuf;
     while(nleft>0){
         if((nwritten = write(fd, bufp, nleft))<0){
            if(errno == EINTR)
                nwritten = 0;
            else
               return -1;
             }
         nleft -= nwritten;
         bufp += nwritten;
     }
    return n;
}

ssize_t rio_read(rio_t *rp, char *userbuf, size_t n){
    int cnt;

    while(rp->rio_cnt <= 0){
        rp->rio_cnt = read(rp->rio_fd, rp->rio_buf, sizeof(rp->rio_buf));
        if(rp->rio_cnt < 0){
            if(errno != EINTR)
                return -1;
        }
        else if(rp->rio_cnt == 0)
            return 0;
        else
            rp->rio_bufptr = rp->rio_buf;
    }
    cnt = n;
    if(rp->rio_cnt<n)
        cnt = rp->rio_cnt;
    memcpy(userbuf, rp->rio_bufptr, cnt);
    rp->rio_bufptr += cnt;
    rp->rio_cnt -= cnt;
    return cnt;
}

ssize_t rio_readlineb(rio_t *rp, void *userbuf, size_t maxlen){
    int n,rc;
    char c, *bufp=userbuf;

    for( n=1; n<maxlen; n++){
        if((rc = rio_read(rp, &c, 1) == 1)){
            *bufp++=c;
            if(c=='\n'){
                n++;
                break;
                }
            }
        else if(rc == 0){
            if(n == 1)
                return 0;
            else
                break;
        }
        else
            return -1;
    }
    *bufp='\0';
    return n-1;
}


