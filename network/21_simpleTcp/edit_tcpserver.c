#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <strings.h>

#define SERV_TCP_PORT  8888  /* TCP Server port */

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

		close( newsockfd );
	}
    close( sockfd );

    return 0;
}
