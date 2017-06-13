/*
 * functions
 * brightness_send   Light state(led brightness)send to SMART PHONE
 * brightness_receive    Receive from smart phone, Brightness set in brightness
 * cds_send	      send the cds sensor information(ON/OFF)
 * ultra_sonic_send      send the ultra sonic sensor value
 */


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termio.h>
#include <linux/ioctl.h>
#include "light.h"
#include "himp.h"

void init_Him_data(Him_data *);


int main(void)
{
	int fd_light,fd_ultra;
	int retn;
	unsigned int i,j,sum=0;
	//unsigned int level;
	char buf[100] = {0,};
	unsigned int sensor_cnt;
	Him_data datas;
	unsigned int value;	



	fd_light =open("/dev/light",O_RDWR);
	printf("fd_light=%d\n",fd_light);
	if(fd_light<0)
	{
		perror("/dev/light error");
		exit(-1);
	}else
		printf("light has been detected..\n");

	fd_ultra =open("/dev/ultra",O_RDWR);
	printf("fd_ultra=%d\n",fd_ultra);
	if(fd_ultra<0)
	{
		perror("/dev/ultra error");
		exit(-1);
	}else
		printf("light has been detected..\n");



	init_Him_data(&datas);
	while(1){
		//getchar();
		sum=0;
		sensor_cnt=0;
		for(i=0;i<15;i++){
			
			for(j=0;j<1000000;j++);
			retn = read(fd_ultra,datas.ultra,32);
			value=(unsigned int)atoi(datas.ultra);

			printf("%u \n",value);
			if(value>10&&value<100)
			{
				sum=sum+value;
				sensor_cnt++;
			}	
		}
		datas.light_level=sum/(sensor_cnt+1);
		printf("%d cm\n",datas.light_level);
		for(j=0;j<1000000;j++);
		if(datas.light_level>100) datas.light_level=99;
		if(datas.light_level<0) datas.light_level=0;
		ioctl(fd_light,DEV_LIGHT_LEVEL,&datas.light_level);
	}
	getchar();
	datas.light_level=100;
	ioctl(fd_light,DEV_LIGHT_LEVEL,&datas.light_level);

	close(fd_light);
	close(fd_ultra);
	printf("light close..\n");
	printf("ultra close..\n");
	return 0;
}

void init_Him_data(Him_data *him)
{
	him->light_level = 0;
	him->light_set   = 0;
	him->cds	 = 0;

}
