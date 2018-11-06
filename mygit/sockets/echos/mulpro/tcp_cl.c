#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <string.h>

#define BUF_SIZE 1024
#define PORT_NUM 5000
#define IP_LEN 20
#define BACK_LOG 5


void getipbyhost(const char* host,char* ip,struct in_addr* ips){
	struct addrinfo *result,*rp;
	if(getaddrinfo(host,NULL,NULL,&result)!=0){
		fprintf(stderr, "getaddrinfo error!\n");
		exit(1);
	}
	for(rp=result;rp!=NULL;rp=rp->ai_next){
		struct sockaddr_in* tmp=(struct sockaddr_in*)rp->ai_addr;
		//char p[20];
		*ips=tmp->sin_addr;
		if(inet_ntop(AF_INET,&tmp->sin_addr,ip,IP_LEN)!=NULL){
			freeaddrinfo(result);
			return;
		}

	}
}


int main(int argc, char const *argv[])
{
	struct sockaddr_in cl,addr;
	ssize_t numRead;
	char buf[BUF_SIZE];
	int cfd,sfd;
	struct in_addr ips;
	char ip[IP_LEN];
	const char *server;

    server=argv[1];
	sfd=socket(AF_INET,SOCK_STREAM,0);
	if(sfd==-1){
		fprintf(stderr, "socket create error!\n" );
		exit(1);
	}
	getipbyhost(server,ip,&ips);
	memset(&addr,0,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_port=PORT_NUM;
	addr.sin_addr=ips;
	if(connect(sfd,(struct sockaddr *)&addr,sizeof(addr))==-1){
		fprintf(stderr, "connect error!\n");
		exit(1);
	}
	 const char* buff=argv[2];

	//printf("Please input the msg:\n");
	//scanf("%s",buf);

	if(write(sfd,buff,strlen(buff))!=strlen(buff)){
		fprintf(stderr ,"write error!\n");
		exit(1);
	}
	memset(buf,0,BUF_SIZE);

	if((numRead=read(sfd,buf,BUF_SIZE))>0){
		printf("%d\n",(int)numRead );
		printf("%s\n",buf);
	}

	if(numRead==-1){
		fprintf(stderr, "read error!\n");
		exit(1);
	}
	memset(buf,0,BUF_SIZE);


	if(close(sfd)==-1){
		fprintf(stderr, "close socket error!\n");
		exit(1);
	}
	return 0;
}