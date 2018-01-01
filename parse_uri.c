#include<string.h>

int parse_uri(char *uri, char *filename, char *cgiargv){
	
	char *ptr;
	if(!strstr(uri, "cgi-bin")){
		strcpy(filename, ".");
		strcpy(cgiargv, "");
		
        /* csapp写法改了uri，这里做一些修改 */
		
        /*if(uri[strlen(uri)-1]=='/')
			strcat(uri, "home.html");
		strcat(filename, uri);*/
		strcat(filename, uri);
		if(uri[strlen(uri)-1]=='/')
			strcat(filename, "index.html");
		return 1;
	}
	else{
		ptr=index(uri, '?');
		if(ptr){
			strcpy(cgiargv, ptr+1);
			*ptr='\0';
		}
		else
			strcpy(cgiargv, "");
		strcpy(filename, ".");
		strcat(filename, uri);
		return 0;
	}
}
