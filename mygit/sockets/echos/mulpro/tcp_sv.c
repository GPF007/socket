#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#define BUF_SIZE 1024
#define PORT_NUM 5000
#define IP_LEN 20
#define BACK_LOG 5
#define MAX_HOST 1025

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

static void sig_handler(int sig){
	int savedErrno;
	savedErrno=errno;
	while(waitpid(-1,NULL,WNOHANG)>0){
		continue;
	}
	errno=savedErrno;
}

static void handler_request(int cfd,struct sockaddr_in* cl){
	char buf[BUF_SIZE];
	ssize_t numRead;
	char remove_host[MAX_HOST];

	if(getnameinfo((struct sockaddr *)cl,sizeof(struct sockaddr),remove_host,MAX_HOST,NULL,0,0)!=0){
			strcpy(remove_host,"UNKNOWN HOST NAME");

	}
	while((numRead=read(cfd,buf,BUF_SIZE))>0){
		printf("Received %d bytes from the client %s \n",(int)numRead,remove_host);
		if(write(cfd,buf,numRead)!=numRead){
			fprintf(stderr, "Write errno!\n");
			exit(1);
		}
	}
	if(numRead==-1){
		fprintf(stderr, "Read errno\n");
		exit(1);
	}
}


int main(int argc, char const *argv[])
{
	ssize_t numRead;
	int sfd,cfd;
	struct sigaction sa;
	struct sockaddr_in sv;
	struct sockaddr_storage cl;
	socklen_t addrlen;
	struct in_addr* ips;	
	char ip[IP_LEN];

	addrlen=sizeof(struct sockaddr_storage);
	sigemptyset(&sa.sa_mask);
	sa.sa_flags=SA_RESTART;
	sa.sa_handler=sig_handler;
	if(sigaction(SIGCHLD,&sa,NULL)==-1){
		fprintf(stderr, "Error from sigaction\n");
		exit(1);
	}


	getipbyhost(argv[1],ip,ips);
	//printf("get ip is %s\n",ip);
	sfd=socket(AF_INET,SOCK_STREAM,0);
	memset(&sv,0,sizeof(sv));
	sv.sin_family=AF_INET;
	sv.sin_port=PORT_NUM;
	sv.sin_addr=*ips;

	if(bind(sfd,(struct sockaddr*)&sv,sizeof(struct sockaddr))!=0){
		fprintf(stderr, "Bind error!\n");
		exit(1);
	}
	printf("bind success!\n");
	if(listen(sfd,BACK_LOG)==-1){
		fprintf(stderr, "Listen error\n");
		exit(1);
	}



	for(;;){
		cfd=accept(sfd,(struct sockaddr*)&cl,&addrlen);
		if(cfd==-1){
			fprintf(stderr, "Accept error\n");
		}
		printf("This is the parent %d\n",(int)getpid());

		switch(fork()){
			case -1:
			      fprintf(stderr, "Create child process failed\n" );
			      close(cfd);
			      break;
			case 0://child
			      close(sfd);
			      printf("This is the child %d\n",(int)getpid());
			      handler_request(cfd,(struct sockaddr_in*)&cl);
			      _exit(0);
			default:
			      close(cfd);
			      break;
		}
	}

	return 0;
}