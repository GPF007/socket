#include <syslog.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define BACKLOG 6
#define BUF_SIZE 500
#define IS_ADDR_STR_LEN 4096
#define PORTNUM 555

int main(int argc, char const *argv[])
{
	int sfd,cfd;
	ssize_t numRead;
	socklen_t addrlen,len;
	struct sockaddr_in svaddr;
	char buf[BUF_SIZE];

	sfd=socket(AF_INET,SOCK_STREAM,0);
	svaddr.sin_family=AF_INET;
	svaddr.sin_port=PORTNUM;
	inet_pton(AF_INET,(char *)gethostbyname("localhost"),&svaddr.sin_addr);

	if(bind(sfd,(struct sockaddr*)&svaddr,sizeof(struct sockaddr_in))==-1)
		exit(errno);

	if(listen(sfd,BACKLOG)==-1)
		exit(errno);

	for(;;){
		cfd=accept(sfd,NULL,NULL);
		if(cfd==-1)
			exit(errno);

		while((numRead=read(cfd,buf,BUF_SIZE))>0){
			printf("%d the message is:%s\n",strlen(buf),buf);
			int  len=strlen(buf);
			if(write(cfd,buf,len)!=len){
				fprintf(stderr, "write errno!\n" );
				exit(errno);
			}
			printf("hhhhhhhh\n");
		}
		if(numRead==-1)
			exit(errno);
		if(close(cfd)==-1)
			exit(errno);

	}
	return 0;
}