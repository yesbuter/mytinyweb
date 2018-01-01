#ifndef _TINY_H_
#define _TINY_H_
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<stdio.h>
#include<netdb.h>
#define MAXLINE 8192
#define RIO_BUFSIZE 8192
typedef struct{
	int rio_fd;
	int rio_cnt;
	char *rio_bufptr;
	char rio_buf[RIO_BUFSIZE];
}rio_t;
extern char **environ;
typedef struct sockaddr SA;
extern void rio_readinitb(rio_t *rp, int fd);
extern ssize_t rio_readlineb(rio_t *rp, void *userbuf, size_t maxlen);
extern ssize_t rio_writen(int fd, void *usrbuf, size_t n);
extern int open_clientfd(char *hostname, char *port);
extern int open_listenfd(char *port);
#endif
