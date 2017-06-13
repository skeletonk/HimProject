#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termio.h>
#include <linux/ioctl.h>
#include "light.h"

int main(void)
{
	int fd;
	int retn;
	unsigned int level;
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
	level=100;
	ioctl(fd,DEV_LIGHT_LEVEL,&level);

	getchar();
	level=10;
	ioctl(fd,DEV_LIGHT_LEVEL,&level);
	getchar();
	level=90;
	ioctl(fd,DEV_LIGHT_LEVEL,&level);
	getchar();
	level=60;
	ioctl(fd,DEV_LIGHT_LEVEL,&level);
	getchar();
	ioctl(fd,DEV_LIGHT_OFF);
	getchar();
	level=30;
	ioctl(fd,DEV_LIGHT_LEVEL,&level);
	getchar();
	level=80;
	ioctl(fd,DEV_LIGHT_LEVEL,&level);

	close(fd);
	printf("light close..\n");
	return 0;
}
