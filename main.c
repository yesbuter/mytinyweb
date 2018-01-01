#include"tiny.h"
extern void doit(int fd);

int main(int argc, char**argv){
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;
	int listenfd, connfd;
	char hostname[MAXLINE], portname[MAXLINE];
	
	if(argc!=2){
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
	}
	
	listenfd = open_listenfd(argv[1]);
	while(1){
		clientlen = sizeof(clientaddr);
		connfd = accept(listenfd, (SA*)&clientaddr, &clientlen);
		getnameinfo((SA*)&clientaddr, clientlen, hostname, MAXLINE, portname, MAXLINE, 0);
		printf("Accept connection from (%s, %s)\n", hostname, portname);
		doit(connfd);
		close(connfd);
	}
}
