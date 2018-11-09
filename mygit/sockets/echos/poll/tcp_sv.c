#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <poll.h>
#include <errno.h>

#define BUF_SIZE 1024
#define PORT_NUM 5000
#define IP_LEN 20
#define BACK_LOG 5
#define MAX_HOST 1025
#define FD_MAX_SIZE 1024

void getaddrbyhost(const char* host,struct in_addr* addr){
	struct addrinfo *result,*rp;
	if(getaddrinfo(host,NULL,NULL,&result)!=0){
		fprintf(stderr, "getaddrinfo error!\n");
		return;
	}
	for(rp=result;rp!=NULL;rp=rp->ai_next){
		struct sockaddr_in* tmp=(struct sockaddr_in*)rp->ai_addr;
		//char p[20];
		*addr=tmp->sin_addr;
		//printf("get that\n");
		return;
	}
}

 struct in_addr* getipbyhost(const char* host){
	struct addrinfo *result,*rp;
	if(getaddrinfo(host,NULL,NULL,&result)!=0){
		fprintf(stderr, "getaddrinfo error!\n");
		exit(1);
	}
	for(rp=result;rp!=NULL;rp=rp->ai_next){
		struct sockaddr_in* tmp=(struct sockaddr_in*)rp->ai_addr;
		//char p[20];
		if(tmp!=NULL)
			freeaddrinfo(result);
		return &tmp->sin_addr;
	}
	return NULL;
}
/*
void getipbyhost(const char* host,char* ip){
	struct addrinfo *result,*rp;
	if(getaddrinfo(host,NULL,NULL,&result)!=0){
		fprintf(stderr, "getaddrinfo error!\n");
		return;
	}
	for(rp=result;rp!=NULL;rp=rp->ai_next){
		struct sockaddr_in* tmp=(struct sockaddr_in*)rp->ai_addr;
		//static char ip[IP_LEN];
		if(inet_ntop(AF_INET,&tmp->sin_addr,ip,IP_LEN)!=NULL){
			freeaddrinfo(result);
			return;
		}
	}
}
*/

int main(int argc, char const *argv[])
{

	struct sockaddr_in sv;
	struct sockaddr_storage cl;
	ssize_t numRead;
	socklen_t addrlen;
	int sfd,cfd;
	struct in_addr* addr;
	char buf[BUF_SIZE];
	addrlen=sizeof(struct sockaddr_storage);
	getaddrbyhost("localhost",addr);
	sfd=socket(AF_INET,SOCK_STREAM,0);
	if(sfd==-1){
		fprintf(stderr, "socket create error\n" );

		exit(1);
	}
	//addr=getipbyhost("localhost");
	
	memset(&sv,0,sizeof(struct sockaddr_in));
	sv.sin_family=AF_INET;
	sv.sin_port=PORT_NUM;
	sv.sin_addr=*addr;

	if(bind(sfd,(struct sockaddr*)&sv,sizeof(struct sockaddr))==-1){
		fprintf(stderr, "bind error\n" );
		fprintf(stderr, "%s\n",strerror(errno));
		exit(1);
	}
	if(listen(sfd,BACK_LOG)==-1){
		fprintf(stderr, "listen error\n" );
	}
	printf("listening\n");
	struct pollfd *pfds;
	int ready,i,j,nfds;
	pfds=calloc(FD_MAX_SIZE,sizeof(struct pollfd));
	if(pfds==NULL){
		fprintf(stderr, "malloc error!\n");
		exit(1);
	}
	memset(pfds,0,sizeof(struct pollfd));

	pfds[sfd].fd=sfd;
	pfds[sfd].events=POLLIN;
	nfds=sfd+1;

	for(;;){
		ready=poll(pfds,nfds,-1);
		if(ready==-1){
			fprintf(stderr, "poll error\n");
			continue;
		}
		//printf("poll\n");
		if(pfds[sfd].revents & POLLIN){//sfd is enable,create a new clinet
			//cfd=accept(sfd,(struct sockaddr*)&cl,&addrlen);
			cfd=accept(sfd,(struct sockaddr*)&cl,&addrlen);
			printf("accepting!\n" );
			if(cfd==-1){
				fprintf(stderr, "accept error!\n");
				continue;
			}
			if(cfd>=FD_MAX_SIZE){
				fprintf(stderr, "cannot accept more clinets\n" );
				continue;
			}

			pfds[cfd].fd=cfd;
			pfds[cfd].events=POLLIN;
			nfds=(cfd+1)>nfds?(cfd+1):nfds;
		}

		//poll the clients
		for(i=0;i<FD_MAX_SIZE;i++){
			if(i==sfd)
				continue;
			if(pfds[i].revents & POLLIN){
				while((numRead=read(i,buf,BUF_SIZE))>0){
					printf("Received %ld bytes\n",(long)numRead);
					if(write(i,buf,numRead)!=numRead){
						fprintf(stderr, "write error\n");
						break;
					}
				}
				memset(buf,0,BUF_SIZE);
				pfds[i].fd=-1;
				pfds[i].events=0;
				close(i);
			}
		}

	}


	return 0;
}