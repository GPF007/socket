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
	struct sockaddr_in svaddr;
	int cfd,sfd,len;
	ssize_t numRead;
	char buf[BUF_SIZE];

	sfd=socket(AF_INET,SOCK_STREAM,0);
	svaddr.sin_family=AF_INET;
	svaddr.sin_port=PORTNUM;
	inet_pton(AF_INET,argv[1],&svaddr.sin_addr);
	if(connect(sfd,(struct sockaddr*)&svaddr,sizeof(struct sockaddr_in))==-1)
		exit(errno);
	int j;
	for(j=2;j<argc;j++){
		len=strlen(argv[j]);
		if(write(sfd,argv[j],len)!=len){
			fprintf(stderr, "errno on write!\n");
			exit(errno);
		}
		numRead=read(sfd,buf,BUF_SIZE);
		printf("%d\n",(int)numRead);
		if(numRead==-1){
			fprintf(stderr, "read errno!\n");
			exit(1);
		}
		//printf("hhhhhhh\n");
		printf("%s\n", buf);
	}
	return 0;
}