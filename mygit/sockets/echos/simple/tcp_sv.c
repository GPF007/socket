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
#define MAX_HOST 1025

void print_addr(const struct sockaddr_in* addr)
{
	char *family;
	switch(addr->sin_family){
		case AF_INET:family="AF_INET";break;
		case AF_INET6:family="AF_INET6";break;
		case AF_UNIX: family="AF_UNIX";break;
		default: family="Unknown family";break;
	}
	printf("sin_family: %s\n",family);
	printf("sin_port: %d\n", addr->sin_port);
	const char *p;
	char ip[20];
	if(inet_ntop(AF_INET,&addr->sin_addr,ip,20)!=NULL)
		printf("sind_addr: %s\n",ip);
}

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
	int sfd;
	ssize_t numRead;
	struct sockaddr_in ad;
	struct sockaddr_storage cl;
	struct addrinfo hints;
	struct addrinfo *result,*rp;
	socklen_t addrlen;
	char buf[BUF_SIZE];
	const char *host;

	sfd=socket(AF_INET,SOCK_STREAM,0);
	if(sfd==-1){
		fprintf(stderr, "socket create error!\n");
		exit(2);
	}
	host=argv[1];
	char ip[IP_LEN];
	struct in_addr ips;
	getipbyhost(host,ip,&ips);
	printf("%s : %s\n", host,ip);

	
	memset(&ad,0,sizeof(ad));
	ad.sin_family=AF_INET;
	ad.sin_port=PORT_NUM;
	ad.sin_addr=ips;
	print_addr(&ad);
	if(bind(sfd,(struct sockaddr*)&ad,sizeof(struct sockaddr))!=0){
		fprintf(stderr, "Bind error!\n");
		return 1;
	}
	if(listen(sfd,BACK_LOG)==-1){
		fprintf(stderr, "Listen error!\n" );
		return 1;
	}
	for(;;){
		int cfd;
		printf("Waiting:\n");
		addrlen=sizeof(struct sockaddr_storage);
		cfd=accept(sfd,(struct sockaddr*)&cl,&addrlen);
		//printf("%d\n",cfd);
		if(cfd==-1){
			fprintf(stderr, "Accept error: \n");
			continue;
		}
		if((numRead=read(cfd,buf,BUF_SIZE))>0){
			//printf("%d\n",numRead);
			printf("%s\n",buf);
			if(write(cfd,buf,numRead)!=numRead){
				fprintf(stderr, "Write error\n");
		    }
		}
		if(numRead==-1){
			fprintf(stderr, "Read error!\n" );
			exit(1);
		}

		char remove_host[MAX_HOST];
		if(getnameinfo((struct sockaddr *)&cl,addrlen,remove_host,MAX_HOST,NULL,0,0)!=0){
			strcpy(remove_host,"UNKNOWN HOST NAME");

		}

		printf("Received: %d bytes from %s\n",(int)numRead,remove_host);

		//print_addr((struct sockaddr_in*)&cl);

		memset(buf,0,sizeof(buf));
		if(close(cfd)==-1){
			fprintf(stderr, "Close socket error!\n" );
			exit(1);
		}
		printf("-----------------------\n");
	}




	return 0;
}