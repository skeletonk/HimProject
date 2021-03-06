/*******************************
 * Filename: server_fork_v2.c
 * Title: Simple TCP Server
 * Desc: 동시성 - 프로세스복제
 *******************************/
 #include "inet.h"
 #include <unistd.h>
 #include <sys/types.h>
 #include <sys/wait.h>

 int main ( int argc, char* argv[] ) {
    int sockfd, newsockfd, clilen;
    struct sockaddr_in  cli_addr;
    struct sockaddr_in  serv_addr;

    int pid;

    char buff[30];
    pname = argv[0];

    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        puts( "Server: 스트림 소켓을 열 수 없습니다.");
        exit(1);
    }

    /* 서버의 주소를 등록하여 클라이언트가 접속 가능하게 한다. */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl( INADDR_ANY );
    serv_addr.sin_port = htons( SERV_TCP_PORT );

    if ( bind( sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0 ) {
        puts( "Server: 지역어드레스로 바인딩 할 수 없습니다.");
        exit(1);
    }

    listen( sockfd, 5 );
    clilen = sizeof( cli_addr );

    /* 자식의 종료시널을 무시한다 */
    signal(SIGCHLD, SIG_IGN);

    while(1) {

        newsockfd = accept( sockfd, (struct sockaddr *) &cli_addr, &clilen );
        if ( newsockfd < 0 ) {
            puts("Server: accept error!"); exit(1);
        }

        pid = fork();

        if (pid < 0 ) {  // fork error
            puts("fork: ERROR on fork!"); exit(1);
        }

        if ( pid == 0 ) {  // child process

            if ( read(newsockfd, buff, 30) <= 0 ) {
                puts( "Server: read error!"); exit(1);
            }
            printf("Server: received message = %s \n", buff);
            close(sockfd);
            close(newsockfd);
            exit(0);
        } /* end of child  */
        close( newsockfd );
    } /* end of while */
    return 0;
}
