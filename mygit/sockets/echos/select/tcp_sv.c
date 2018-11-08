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
#define FD_MAX_SIZE 1024


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


int main(int argc, char const *argv[])
{
	/* code */

	struct sockaddr_in sv;
	struct sockaddr_storage cl;
	ssize_t numRead;
	socklen_t addrlen;
	int sfd,cfd;
	struct in_addr* addr;
	char buf[BUF_SIZE];
	addr=getipbyhost("localhost");
	sfd=socket(AF_INET,SOCK_STREAM,0);
	if(sfd==-1){
		fprintf(stderr, "socket create error\n" );
		exit(1);
	}
	memset(&sv,0,sizeof(struct sockaddr_in));
	sv.sin_family=AF_INET;
	sv.sin_port=PORT_NUM;
	sv.sin_addr=*addr;
	if(bind(sfd,(struct sockaddr*)&sv,sizeof(struct sockaddr))!=0){
		fprintf(stderr, "bind error\n" );
		exit(1);
	}
	if(listen(sfd,BACK_LOG)==-1){
		fprintf(stderr, "listen error\n" );
	}

	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(sfd,&readfds);
	int max_fd=sfd+1;
	int ready;
	int fds[FD_MAX_SIZE];
	int i;
	memset(fds,-1,sizeof(fds));
	for(;;){
		printf("waiting:-------------------\n");
		ready=select(max_fd,&readfds,NULL,NULL,NULL);
		printf("%d\n", ready);
		if(ready<0){
			fprintf(stderr, "select error\n");
			continue;
		}
		//printf("sds\n");
		if(FD_ISSET(sfd,&readfds)==1){//if the listened fd can be used ,accept new clinet

			cfd=accept(sfd,(struct sockaddr*)&cl,&addrlen);
			char remove_host[MAX_HOST];
		    if(getnameinfo((struct sockaddr *)&cl,addrlen,remove_host,MAX_HOST,NULL,0,0)!=0)
			     strcpy(remove_host,"UNKNOWN HOST NAME");
		    printf("accept new client from %s\n",remove_host);
		    if(cfd>=FD_MAX_SIZE){
		    	fprintf(stderr, "Cannot accept new clinets!\n" );
		    	continue;
		    }
		    fds[cfd]=1;
			FD_SET(cfd,&readfds);
			max_fd=max_fd > (cfd+1)?max_fd:(cfd+1);
			//printf("max_fd : %d\n",max_fd );
		}
		for(i=0;i<FD_MAX_SIZE;i++){//poll all the clients fds
			if(FD_ISSET(i,&readfds)==1 && fds[i]==1){
				while((numRead=read(i,buf,BUF_SIZE))>0){
					printf("read %ld bytes.\n",numRead );
					if(write(i,buf,numRead)!=numRead){
						fprintf(stderr, "write error!\n" );
						break;
					}
				}
				fds[i]=-1;
				memset(buf,0,BUF_SIZE);
				if(numRead==-1){
					fprintf(stderr, "read error\n");
					continue;
				}
				FD_CLR(i,&readfds);
				close(i);
			}
		}
	}
	return 0;
}