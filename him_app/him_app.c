#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <strings.h>

#include <unistd.h>
#include <fcntl.h>
#include <termio.h>
#include <linux/ioctl.h>
#include "light.h"
#include "himp.h"

#define SERV_TCP_PORT  8888  /* TCP Server port */
//192.168.0.199
//123.214.186.212 8888


/*
 * functions
 * 1) brightness_send       Light state(led brightness)send to SMART PHONE
 * 2) brightness_receive    Receive from smart phone, Brightness set in brightness
 * 3) cds_send	      	    send the cds sensor information(ON/OFF)
 * 4) ultra_sonic_send      send the ultra sonic sensor value
 */


void init_Him_data(Him_data *hm);
void brightness_receive(Him_data *hm, int sock_d, int fd_light);
void Him_status_send(Him_data *hm,int sock_fd,int fd_ultra, int fd_light);


int main ( int argc, char* argv[] ) {
    	int sockfd, newsockfd, clilen;
    	struct sockaddr_in  cli_addr;
	struct sockaddr_in  serv_addr;
    	char buff[30];
   	int size;
 
	//++
	char *str_1;
	char *str_2;
	//++

	int fd_light,fd_ultra;
	int retn;
	unsigned int i,j;
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


	//create tcp socket to get sockfd
    	if ( (sockfd = socket(AF_INET, SOCK_STREAM,0)) < 0 ) {
        	puts( "Server: Cannot open Stream Socket.");
        	exit(1);
	}
    	bzero((void *) &serv_addr, sizeof(serv_addr));
    	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl( INADDR_ANY );
    
	//++
	printf("INADDR_ANY : %s\n", INADDR_ANY);
	str_1 = inet_ntoa(serv_addr.sin_addr);
	str_2 = inet_ntoa(cli_addr.sin_addr);

	printf("%s\n", str_1);
	printf("%s\n", str_2);
	//++

	//set port
	serv_addr.sin_port = htons(SERV_TCP_PORT);
 	//bind your socket with the address info   
    	if ( bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr))<0 ) {
        	puts( "Server: Cannot bind Local Address.");
        	exit(1);
    		}
	//set listen args    
    	listen(sockfd, 5);
    	clilen = sizeof( cli_addr );
	while(1) {
	//call accept	
        	newsockfd = accept(sockfd,(struct sockaddr*) &cli_addr, &clilen);
	        if ( newsockfd < 0 ) {
			puts("Server: accept error!");
			exit(1);
		}
    
		if ((size = read(newsockfd, buff, 20)) <= 0 ) {
			puts( "Server: readn error!");
			exit(1);
		}
		printf("reading newsockfd from Client = %d\n", size);
		printf("Server: Received String = %s \n", buff);
		
		//++
		if ( write(newsockfd, "It's so sunny day!", 20+1) < 21 ) {
			puts( "Server: written error" );
			exit(1);
		}
		//++
		//-------------------App------------------------//

		while(1){
			Him_status_send(&datas,newsockfd,fd_ultra,fd_light);
			printf("status\n");
			printf("cds status : %u\n",datas.cds);
			printf("light_level : %u\n",datas.light_level);
			printf("ultra sonic sensor state : %u cm\n",datas.ultra);

			brightness_receive(&datas,newsockfd,fd_light);
			if(datas.light_set>100||datas.light_set<21) datas.light_set = 100;
			else datas.light_set-=20;
		
			if(datas.ultra<130&&datas.ultra>15) 
				ioctl(fd_light,DEV_LIGHT_LEVEL,&datas.light_set);
			else 
				ioctl(fd_light,DEV_LIGHT_LEVEL,0);

			getchar();
		}
		getchar();
		/* -------------application area ---------------------*/
		close(fd_light);
		close(fd_ultra);
		printf("light close..\n");
		printf("ultra close..\n");
		close( newsockfd );
	}
    	close( sockfd );

    	return 0;
}

	
	/* ------------- application area --------------------*/

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
	//TODO receive func

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
