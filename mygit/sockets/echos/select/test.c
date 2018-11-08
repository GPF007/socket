#include <sys/time.h>
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
	fd_set readfds,writefds;
	int ready,nfds,fd,numRead,j;
	struct timeval timeout;


	timeout.tv_sec=5;
	timeout.tv_usec=0;

	nfds=0;
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);

	//monitor the read descripter 0;
	fd=0;
	FD_SET(0,&readfds);

	//monitor the write 1
	fd=1;
	FD_SET(1,&readfds);

	nfds=2;

	ready=select(nfds,&readfds,&writefds,NULL,&timeout);
	if(ready==-1){
		fprintf(stderr, "select wrong\n");
		exit(1);
	}

	//Display the result
	printf("the ready number is%d\n",ready);
	for(fd=0;fd<nfds;fd++){
		printf("%d:%s%s\n",fd,FD_ISSET(fd,&readfds)?"r":"",FD_ISSET(fd,&writefds)?" w":"" );
		printf("time after select () %ld.%03ld\n",
		        (long)timeout.tv_sec,(long)timeout.tv_usec/10000);
	}

	printf("Hello world!\n");
	return 0;
}