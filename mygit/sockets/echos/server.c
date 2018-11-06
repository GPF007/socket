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
	/* code */

	int sfd;
	ssize_t numRead;
	socklen_t addrlen,len;
	struct sockaddr_storage claddr;
	struct sockaddr_in svaddr;
	char buf[BUF_SIZE];
	char addStr[IS_ADDR_STR_LEN];

    inet_pton(AF_INET,(char *)gethostbyname("localhost"),&svaddr.sin_addr);

	sfd=socket(AF_INET,SOCK_DGRAM,0);
	svaddr.sin_family=AF_INET;
	//svaddr.sin_addr=INADDR_ANY;
	svaddr.sin_port=777;

	if(bind(sfd,(struct sockaddr *) &svaddr,sizeof(struct sockaddr_in))==-1)
		exit(errno);

	for(;;){
		len=sizeof(struct sockaddr_in);
		numRead=recvfrom(sfd,buf,BUF_SIZE,0,(struct sockaddr *) &claddr,&len);
		printf("%s\n",buf);
		if(numRead==-1)
			exit(errno);
		if(sendto(sfd,buf,numRead,0,(struct sockaddr *)&claddr,len)!=numRead)
			exit(errno);
	}

	return 0;
}
