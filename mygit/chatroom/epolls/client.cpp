#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <sys/epoll.h>
#include <errno.h>
#include <fcntl.h>

#include <iostream>
#include <list>
#include <queue>
#include <string>

#define BUF_SIZE 1024
#define PORT_NUM 5001
#define IP_LEN 20
#define BACK_LOG 5
#define MAX_HOST 1025
#define MAXEPOLL 1000
#define MAX_EVENTS 100
#define EPOLL_SIZE 2

using namespace std;

void setnonblock(int sock)  
{  
    int opts;  
    opts=fcntl(sock,F_GETFL);  
 
    if(opts<0)  
    {  
        perror("fcntl(sock,GETFL)");  
        exit(1);  
    }  
 
    opts = opts | O_NONBLOCK;  
 
    if(fcntl(sock,F_SETFL,opts)<0)  
    {  
        perror("fcntl(sock,SETFL,opts)");  
        exit(1);  
    }  
}

class Client{
public:

	void init(){

		if((sfd=socket(AF_INET,SOCK_STREAM,0))<0){
			fprintf(stderr, "socket create error!\n");
			exit(1);
		}
		if(inet_pton(AF_INET,"127.0.0.1",(void *)&ip)!=1){
			fprintf(stderr, "convert ip error!\n");
			exit(1);
		}

		memset(&ad,0,sizeof(ad));
	    ad.sin_family=AF_INET;
	    ad.sin_port=PORT_NUM;
	    ad.sin_addr=ip;

	}

	void run(){
		if(connect(sfd,(struct sockaddr*)&ad,sizeof(ad))==-1){
		    fprintf(stderr, "connect error\n" );
		    exit(1);
     	}
     	//int pipe_fd[2];
     	if(pipe(pipe_fd)<0){
     		fprintf(stderr, "pipe create error!\n" );
     		exit(1);
     	}

     	epfd=epoll_create(EPOLL_SIZE);
     	if(epfd<0){
     		fprintf(stderr, "epoll_create error!\n");
     		exit(1);
     	}
     	ev.data.fd=sfd;
     	ev.events=EPOLLIN | EPOLLET;
     	epoll_ctl(epfd,EPOLL_CTL_ADD,sfd,&ev);
     	setnonblock(sfd);

     	ev.data.fd=pipe_fd[0];
     	ev.events=EPOLLIN | EPOLLET;
     	epoll_ctl(epfd,EPOLL_CTL_ADD,pipe_fd[0],&ev);

     	int pid=fork();
     	if(pid<0){
     		fprintf(stderr, "fork error\n" );
     		exit(1);
     	}else if(pid==0){
     		//child process
     		child_handle();
     	}else{
     		parent_handle();
     	}

     	if(pid){
     		close(pipe_fd[0]);
     		close(sfd);

     	}else{
     		close(pipe_fd[1]);
     	}



     	//handle();
	}


	void child_handle(){
		close(pipe_fd[0]);
		printf("Please input enter to exit the chat room\n");
		memset(buf,0,BUF_SIZE);

		while(cin.getline(buf,BUF_SIZE)){
			if(buf[0]=='\0'){
				is_work=false;
				break;
			}

			if(write(pipe_fd[1],buf,strlen(buf))<0){
				fprintf(stderr, "write error!\n");
				exit(1);
			}
			is_work=true;
		}
	}

	void parent_handle(){
		close(pipe_fd[1]);

		while(is_work){
			nfds=epoll_wait(epfd,events,EPOLL_SIZE,-1);

			for(int i=0;i<nfds;++i){
				memset(buf,0,BUF_SIZE);

				if(events[i].data.fd==sfd){
					numRead=read(sfd,buf,BUF_SIZE);
					printf("%s\n",buf);
				}else{
					numRead=read(events[i].data.fd,buf,BUF_SIZE);
					if(numRead==0) is_work=false;
					else{
						write(sfd,buf,BUF_SIZE);
					}
				}
			}
		}
	}

	void handle(){

		memset(buf,0,BUF_SIZE);

		while(cin.getline(buf,BUF_SIZE)){
			if(buf[0]=='\0') break;

			if(write(sfd,buf,strlen(buf))!=strlen(buf)){
				fprintf(stderr, "write error\n");
				exit(1);
			}
			if((numRead=read(sfd,buf,BUF_SIZE))>0){
				printf("%s\n",buf );
			}
			if(numRead==-1){
				fprintf(stderr, "read error\n" );
				exit(1);
			}
			memset(buf,0,BUF_SIZE);
		}

		close(sfd);

	}

private:
	/*
	int cfd,sfd;
	ssize_t numRead;
	char buf[BUF_SIZE];
	struct in_addr ip;
	struct sockaddr_in cl,ad;

	char username[BUF_SIZE];
	*/

	int sfd;
	struct sockaddr_in ad;
	ssize_t numRead;
	char buf[BUF_SIZE];

	struct in_addr ip;

	int epfd,nfds;
	struct epoll_event ev,events[2];
	int pipe_fd[2];
	bool is_work;
};

int main(int argc, char const *argv[])
{
	printf("hello world!\n");

	Client client;
	client.init();
	client.run();
	return 0;
}