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
#include <pthread.h>
#include <semaphore.h>
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
struct pt_data{
	Him_data hm;
	int fd_light;
	int fd_ultra;
	int socksfd;
};
pthread_mutex_t mutex =PTHREAD_MUTEX_INITIALIZER;
void init_Him_data(Him_data *hm);
void brightness_receive(Him_data *hm, int sock_d, int fd_light);
void Him_status_send(Him_data *hm,int sock_fd,int fd_ultra, int fd_light);
void *threadfunc(struct pt_data *pdata);
void set_light_value(Him_data *hm,int fd_light,int fd_ultra);


Him_data datas;
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
	struct pt_data pt_data;
	pthread_t thread_t;




	fd_ultra =open("/dev/ultra",O_RDWR);
	printf("fd_ultra=%d\n",fd_ultra);
	if(fd_ultra<0)
	{
		perror("/dev/ultra error");
		exit(-1);
	}else
		printf("light has been detected..\n");

	fd_light =open("/dev/light",O_RDWR);
	printf("fd_light=%d\n",fd_light);
	if(fd_light<0)
	{
		perror("/dev/light error");
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
			//exit(1);
			break;
		}

		pt_data.hm=datas;
		pt_data.fd_light=fd_light;
		pt_data.fd_ultra=fd_ultra;
		pt_data.socksfd=newsockfd;

		if((pthread_create(&thread_t,0,(void*)threadfunc,&pt_data))<0)
		{
			puts("thread create error:\n");
			exit(0);
		}
		while(1){


			int temp;
			if ((size = read(newsockfd,buff,20+1)) <= 0 ) {
				puts( "Server: readn error!");
				exit(1);
			}
			printf("%s\n",buff);
				
			printf("reading newsockfd from Client = %d\n", size);
			printf("Server: Received String = %s \n", buff);
			if(strcmp(buff,"end")==0)
			{
				printf("end of sock %d\n",newsockfd);
				break;
			}
			temp=atoi(buff);
			printf("%u\n",temp);

		//for(i=0;i<15;i++){	
		//	for(j=0;j<1000000;j++);
		//	read(fd_ultra,&datas.ultra,sizeof(datas.ultra));
		//	if(datas.ultra>10&&datas.ultra<200)
		//	{
		//		sum=sum+(datas.ultra);
		//		sensor_cnt++;
		//	}	
		//}

		//datas.ultra=sum/(sensor_cnt+1);
		datas.light_set=temp;

		
		}


	}
	pthread_join(thread_t,0);
	close(fd_light);
	close(fd_ultra);
	printf("light close..\n");
	printf("ultra close..\n");
    	close( sockfd );

    	return 0;
}
void *threadfunc(struct pt_data *pdata)
{
	int newsockfd;
	int size,i;
	char buff[30]={0,};
	newsockfd=*(int*)&pdata->socksfd;

	  	while(1){
			for(i=0;i<1000000;i++);
		Him_status_send(&pdata->hm, newsockfd, pdata->fd_ultra, pdata->fd_light);
		printf("%u, %u,%u\n",pdata->hm.ultra, pdata->hm.light_level,pdata->hm.cds);

		if(datas.light_set>100) datas.light_set=99;
		if(datas.light_set<0) datas.light_set=0;	

		printf("datas.ultra %u\n",datas.ultra);
		printf("datas.light_level %u\n",datas.light_level);
		printf("datas.cds %u\n",datas.cds);
		printf("datas.light_set %u\n",datas.light_set);

		set_light_value(&datas,pdata->fd_light,pdata->fd_ultra);

	}
	printf("close newsockfd\n");
	close( newsockfd );
}
	
	/* ------------- application area --------------------*/

void Him_status_send(Him_data *hm,int sock_fd,int fd_ultra, int fd_light)
{	
	static int i,j;
	unsigned int sum=0;
	unsigned int sensor_cnt=0;
	unsigned int value=0;
	unsigned int temp;
	char buf[50]={0,};
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
	sprintf(buf,"%u cm, cds : %u, led brightness : %u\n",hm->ultra,hm->cds,hm->light_level);
	//TODO send.........
	if ( write(sock_fd, buf, 45+1) < 46 ) {
		puts( "Server: written error" );
		exit(1);
	}

}
void brightness_receive(Him_data *hm, int sock_d, int fd_light)
{
	Him_data temp;
	char buff[30]={0,};
	int size;
	temp.light_level=hm->light_level;
	temp.light_set = hm->light_set;
	temp.cds=hm->cds;
	temp.ultra=hm->ultra;
	//TODO receive func

	if ((size = read(sock_d,buff,20+1)) <= 0 ) {
		puts( "Server: readn error!");
		exit(1);
	}
	printf("%s\n",buff);
	hm->light_set=atoi(buff);
	printf("%d\n",hm->light_set);
	if(hm->light_set>100) hm->light_set=99;
	if(hm->light_set<0) hm->light_set=0;
	hm->light_level=temp.light_level;
	hm->cds=temp.cds;
	hm->ultra=temp.ultra;
}
void set_light_value(Him_data *hm,int fd_light,int fd_ultra)
{
	pthread_mutex_lock(&mutex);
	if(datas.ultra>110) datas.light_set = 1;
	ioctl(fd_light,DEV_LIGHT_LEVEL,&datas.light_set);
	pthread_mutex_unlock(&mutex);
}
void init_Him_data(Him_data *him)
{
	him->light_level = 0;
	him->light_set   = 0;
	him->cds	 = 0;
	him->ultra	 = 0;
}
