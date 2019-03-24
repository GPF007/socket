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

class Server{

public:
    void init(){
        if((sfd=socket(AF_INET,SOCK_STREAM,0))==-1){
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

        if(bind(sfd,(struct sockaddr*)&ad,sizeof(struct sockaddr))!=0){
             fprintf(stderr, "bind error!\n");
              exit(1);
        }

        addrlen=sizeof(struct sockaddr_storage);
    }

    void run(){
        if(listen(sfd,BACK_LOG)==-1){
            fprintf(stderr, "listen error\n" );
            exit(1);
        }
        setnonblock(sfd);

        epollfd=epoll_create(MAX_EVENTS);
        if(epollfd==-1){
            fprintf(stderr, "epoll_create error\n" );
            exit(1);
        }
        ev.events=EPOLLIN | EPOLLET;
        ev.data.fd=sfd;
        if(epoll_ctl(epollfd,EPOLL_CTL_ADD,sfd,&ev)==-1){
            fprintf(stderr, "epoll_ctl: add sfd error!\n");
            exit(1);
        }


        for(;;){
            printf("waiting:\n");
            nfds=epoll_wait(epollfd,events,MAX_EVENTS,-1);
            if(nfds==-1){
                fprintf(stderr, "epoll_wait\n" );
                exit(1);
            }
            int n;

            for(n=0;n<nfds;++n){
                if(events[n].data.fd == sfd){
                    //accept a new client
                    cfd=accept(sfd,(struct sockaddr *)&cl,&addrlen);

                    if(cfd==-1){
                        fprintf(stderr, "accept error!\n");
                        exit(1);
                    }
                    setnonblock(cfd);
                    ev.events=EPOLLIN | EPOLLET;
                    ev.data.fd=cfd;
                    if(epoll_ctl(epollfd,EPOLL_CTL_ADD,cfd,&ev)==-1){
                        fprintf(stderr, "epoll_ctl: add cfd error!\n");
                        exit(1);

                    }
                    //printf("receive a new client from\n", );
                    clientfd.push_back(cfd);
                    printf("receive a new client fd=%d\n",cfd);
                    snprintf(buf,BUF_SIZE,"welcome client %d!",cfd);
                    write(cfd,buf,strlen(buf));

                }else if(events[n].events & EPOLLIN){

                    //fd could read
                    fd=events[n].data.fd;
                    if(fd==-1) continue;

                    send_broadcast(fd);

                }
            }


        }
    }


    void send_broadcast(int fd){
        memset(buf,0,BUF_SIZE);
        numRead=read(fd,buf,BUF_SIZE);
        //char prefix[BUF_SIZE];
        now = time(NULL);
        timestamp=ctime(&now);
        timestamp[strlen(timestamp)-1]='\0';
        snprintf(msg,BUF_SIZE,"[%s] ",timestamp);

        if(numRead==0){
            snprintf(temp,BUF_SIZE,"clinet %d is out!",fd);
            strcat(msg,temp);
            clientfd.remove(fd);
            ev.data.fd=fd;
            if(epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,&ev)==-1){
                fprintf(stderr, "epoll_ctl:del closed fd error!\n" );
                exit(1);
            }
            close(fd);
        }else{

            snprintf(temp,BUF_SIZE,"client%d:",fd);
            strcat(msg,temp);
            strcat(msg,buf);
        }


        //boardcast the message
        list<int>::iterator it;
        for(it=clientfd.begin();it!=clientfd.end();it++){
            if(write(*it,msg,strlen(msg))!=strlen(msg)){
                fprintf(stderr, "write error\n");
            }
        }


    }

private:
    int sfd,cfd;
    struct sockaddr_in ad;
    struct sockaddr_storage cl;
    socklen_t addrlen;
    ssize_t numRead;
    char buf[BUF_SIZE];
    char msg[BUF_SIZE];
    char temp[BUF_SIZE];
    struct in_addr ip;

    struct epoll_event ev,events[MAX_EVENTS];
    int nfds,epollfd,fd;

    list<int> clientfd;

    time_t now;
    char *timestamp;



};

int main(int argc, char const *argv[])
{
    printf("hello world!\n");
    Server server;
    server.init();
    server.run();
    return 0;
}