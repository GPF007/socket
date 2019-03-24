#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char const *argv[])
{
	//char prefix[1024]="client";

	char buf[1024];
	//buf=ctime(time(NULL));
	time_t now=time(NULL);
	char *str=ctime(&now);
	strcpy(buf,str);
	buf[strlen(buf)-1]='\0';
	printf("%s\n",buf );
	return 0;
}