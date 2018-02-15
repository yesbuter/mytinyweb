#include"tiny.h"

//解析uri并得到文件名和参数
extern int parse_uri(char *uri, char *filename, char *cgiargv);
//处理静态请求
extern void serve_static(int fd, char *filename, int filesize);
//处理动态请求
extern void serve_dynamic(int fd, char *filename, char *cgiargv);
//出错处理
extern void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);

//只实现GET方法，因此跳过客户发来的除了头部行的其他信息
void dorequest(rio_t *rp){
    char buf[MAXLINE];
    rio_readlineb(rp, buf, MAXLINE);
    while(strcmp(buf, "\r\n")){
        printf("%s\n", buf);
        rio_readlineb(rp, buf, MAXLINE);
    }
    printf("%s\n", buf);
    return;
}

void doit(int fd){
    int is_static;
    char buf[MAXLINE], method[MAXLINE], version[MAXLINE], uri[MAXLINE];
    char filename[MAXLINE], cgiargv[MAXLINE];
    struct stat sbuf;
    rio_t rp;

    rio_readinitb(&rp, fd);
    //获取头部行
    rio_readlineb(&rp, buf, MAXLINE);
    printf("Request headers:\n");
    printf("%s", buf);
    //通过头部行读入method, uri, version
    sscanf(buf, "%s %s %s", method, uri, version);
    //只支持GET方法
    if(strcasecmp(method, "GET")){
        clienterror(fd, method, "501", "Not implemented", "Tiny does not implement this method");
        return ;
    }
    //跳过其他信息
    dorequest(&rp);

    //解析uri得到文件名和参数和是否是静态请求的标识
    is_static=parse_uri(uri, filename, cgiargv);
    //如果请求的文件不存在，调用错误处理
    if(stat(filename, &sbuf)<0){
        clienterror(fd, filename, "404", "Not Found", "Tiny couldn't find this file");
        return;
    }

    //如果是静态文件
    if(is_static){
        //如果没权限，调用错误处理
        if(!(S_ISREG(sbuf.st_mode))||!(S_IRUSR&sbuf.st_mode)){
            clienterror(fd, filename, "403", "Forbidden", "Tiny couldn't read the file");
            return;
        }
        //调用静态服务
        serve_static(fd, filename, sbuf.st_size);
    }
    //如果是动态文件
    else{
        //如果没有权限，调用错误处理
        if(!(S_ISREG(sbuf.st_mode))||!(S_IXUSR&sbuf.st_mode)){
            clienterror(fd, filename, "403", "Forbidden", "Tiny couldn't run the CGI program");
            return;
        }
        //调用动态服务
        serve_dynamic(fd, filename, cgiargv);
    }
}

