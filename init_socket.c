#include <sys/socket.h>
#include <netinet/in.h> 
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include "init_socket.h"

void init_socket(int *sd, char *str,int portnum, struct sockaddr_in *sinn){ // 소켓 초기화
    if((*sd=socket(AF_INET,SOCK_STREAM,0))==-1){ // client 소캣
        perror("socket");
        exit(1);
    }
    memset((char*)sinn,'\0',sizeof(*sinn)); // 소켓 주소 구조체 초기화
    sinn->sin_family = AF_INET; // 소켓 주소 구조체에 인터넷 소켓임을 알림
    sinn->sin_port = htons(portnum); // 포트번호
    sinn->sin_addr.s_addr = inet_addr(str); //ip주소를 숫자로 변환
}