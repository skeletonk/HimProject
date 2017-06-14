#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <strings.h>

//define port and address of your server
#define SERV_TCP_PORT   8888    /* TCP Server port */
#define SERV_HOST_ADDR  "192.168.0.102" /* Server IP address */

 int main ( int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in serv_addr;

	//++
//	int newsockfd, clilen;
	char buff[30];
//	int size;
//	struct sockaddr_in cli_addr;
	//++

    bzero((void *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
//port setting
	serv_addr.sin_addr.s_addr = inet_addr(SERV_HOST_ADDR);
	serv_addr.sin_port = htons(SERV_TCP_PORT);
    
    /* TCP Socket Open */
    if ((sockfd = socket( AF_INET, SOCK_STREAM, 0 )) < 0 ) {
        puts( "Client: Cannot open Stream Socket.");
        exit(1);
    }
    
//connect to server
    if ( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0) {
        puts( "Client: Cannot connect the Server.");
        exit(1);
    }

	//++
//	while(1) {
	    if ( write( sockfd, "How is the weather?", 20) < 20 ) {
    	    puts( "Client: written error" );
	        exit(1);
	    }

		read( sockfd, buff, 21); 
    
		printf("reading newsockfd from Server = %s\n", buff);

//	}
	//++

//close socket
    close( sockfd );
	return 0;
}
