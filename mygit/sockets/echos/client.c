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

#define BUF_SIZE 500
#define IS_ADDR_STR_LEN 4096

int main(int argc, char const *argv[])
{
	
	int sfd,j;
	size_t len;
	ssize_t numRead;
	char buf[BUF_SIZE];
	struct sockaddr_in svaddr;
	sfd=socket(AF_INET,SOCK_DGRAM,0);
	if(sfd==-1)
		exit(errno);

	inet_pton(AF_INET,argv[1],&svaddr.sin_addr);
	svaddr.sin_family=AF_INET;
	//svaddr.sin_addr=INADDR_ANY;
	svaddr.sin_port=777;
	if(connect(sfd,(struct sockaddr*)&svaddr,sizeof(struct sockaddr_in))==-1)
		exit(errno);

	for(j=2;j<argc;j++){
		len=strlen(argv[j]);
		if(write(sfd,argv[j],len)!=len)
			exit(errno);
		numRead=read(sfd,buf,BUF_SIZE);

		printf("[%ld bytes] %.*s\n",(long)numRead,(int)numRead,buf);
	}

	return 0;
}