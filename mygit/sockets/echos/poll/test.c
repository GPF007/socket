#include <time.h>
#include <poll.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
	struct pollfd* pfd;
	int i,n,ready;
	//monitor fd 0
	pfd=calloc(1,sizeof(struct pollfd));
	pfd[0].fd=0;
	pfd[0].events=POLLIN;
	printf("asdasda\n");
	/*
	//monitor fd 1
	pfd[1].fd=1;
	pfd[1].events=POLLOUT;
	*/
	n=1;
	ready=poll(pfd,n,-1);
	if(ready==-1){
		fprintf(stderr, "poll error\n" );
		exit(1);
	}
	for(i=0;i<n;i++){
		if(pfd[i].revents & POLLIN){
			printf("Readable: %d %3d\n",i,pfd[i].fd);
		}
	}
	return 0;
}