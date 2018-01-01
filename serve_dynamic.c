#include"tiny.h"
#include<sys/wait.h>
#include<string.h>

void serve_dynamic(int fd, char *filename, char *cgiargv){
	char buf[MAXLINE], *emptylist[] = {NULL, };

	sprintf(buf, "HTTP/1.0 200 OK\r\n");
	rio_writen(fd, buf, strlen(buf));
	sprintf(buf, "Server: Tiny Web Server\r\n");
	rio_writen(fd, buf, strlen(buf));

	if(fork() == 0){
		setenv("QUERY_STRING", cgiargv, 1);
		dup2(fd, STDOUT_FILENO);
		execve(filename, emptylist, environ);
	}
	wait(NULL);
}
