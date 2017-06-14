/*
 * functions
 * 1) brightness_send       Light state(led brightness)send to SMART PHONE
 * 2) brightness_receive    Receive from smart phone, Brightness set in brightness
 * 3) cds_send	      	    send the cds sensor information(ON/OFF)
 * 4) ultra_sonic_send      send the ultra sonic sensor value
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

void init_Him_data(Him_data *hm);
void brightness_receive(Him_data *hm, int sock_d, int fd_light);
void Him_status_send(Him_data *hm,int sock_fd,int fd_ultra, int fd_light);
int main(void)
{
	int fd_light,fd_ultra;
	int retn;
	unsigned int i,j;
	char buf[100] = {0,};
	Him_data datas;



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
	
	/* ------------- application area --------------------*/
	while(1){
		Him_status_send(&datas,1,fd_ultra,fd_light);
		printf("status\n");
		printf("cds status : %u\n",datas.cds);
		printf("light_level : %u\n",datas.light_level);
		printf("ultra sonic sensor state : %u cm\n",datas.ultra);

		brightness_receive(&datas,1,fd_light);
		if(datas.light_set>100||datas.light_set<21) datas.light_set = 100;
		else datas.light_set-=20;
		
		if(datas.ultra<130&&datas.ultra>15) ioctl(fd_light,DEV_LIGHT_LEVEL,&datas.light_set);
		else ioctl(fd_light,DEV_LIGHT_LEVEL,0);

		getchar();
	}
	getchar();
	/* -------------application area ---------------------*/
	close(fd_light);
	close(fd_ultra);
	printf("light close..\n");
	printf("ultra close..\n");
	return 0;
}
void Him_status_send(Him_data *hm,int sock_fd,int fd_ultra, int fd_light)
{	
	static int i,j;
	unsigned int sum=0;
	unsigned int sensor_cnt=0;
	unsigned int value=0;
	unsigned int temp;
	temp=hm->light_set;
	read(fd_light,hm,sizeof(Him_data));
	for(i=0;i<15;i++){	
		for(j=0;j<1000000;j++);
		read(fd_ultra,&hm->ultra,sizeof(hm->ultra));
		if(hm->ultra>10&&hm->ultra<200)
		{
			sum=sum+(hm->ultra);
			sensor_cnt++;
		}	
	}
	hm->ultra=sum/(sensor_cnt+1);
	hm->light_set=temp;
	//TODO send.........


}
void brightness_receive(Him_data *hm, int sock_d, int fd_light)
{
	Him_data temp;
	temp.light_level=hm->light_level;
	temp.light_set = hm->light_set;
	temp.cds=hm->cds;
	temp.ultra=hm->ultra;
	//hm.light_set=

	if(hm->light_set>100) hm->light_set=99;
	if(hm->light_set<0) hm->light_set=0;
	hm->light_level=temp.light_level;
	hm->cds=temp.cds;
	hm->ultra=temp.ultra;
}

void init_Him_data(Him_data *him)
{
	him->light_level = 0;
	him->light_set   = 0;
	him->cds	 = 0;
	him->ultra	 = 0;
}
