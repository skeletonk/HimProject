#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>


int main(void)
{
	int fd;
	int retn;
	char buf[100] = {0,};


	fd =open("/dev/light",O_RDWR);
	printf("fd=%d\n",fd);
	if(fd<0)
	{
		perror("/dev/light error");
		exit(-1);
	}else
		printf("light has been detected..\n");
	getchar();

	close(fd);
	printf("light close..\n");
	return 0;
}
