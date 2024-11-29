#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PORTNUM 9005

// UDP 동작 서버 
int main(){
    char buf[BUFSIZ],name[20];
    struct sockaddr_in sin;
    int sd, n;
    
    // 인터넷 소켓, UDP 사용
    if((sd=socket(AF_INET,SOCK_STREAM,0))==-1){ // 서버측 udp 소켓
        perror("socket");
        exit(1);
    }

    memset((char*)&sin,'\0',sizeof(sin)); // 소켓 주소 구조체 초기화

    sin.sin_family = AF_INET; // 소켓 주소 구조체에 유닉스 도메인 소켓임을 알림
    sin.sin_port = htons(PORTNUM);
    sin.sin_addr.s_addr = inet_addr("192.168.56.1");   

    if(connect(sd,(struct sockaddr*)&sin,sizeof(sin))){
        perror("connect");
        exit(1);
    }

    if(recv(sd,buf,sizeof(buf),0)==-1){
        perror("recv");
        exit(1);
    }
    printf("%s\n",buf);
    scanf("%s",name);
    strcpy(buf,name);
    if(send(sd,buf,sizeof(buf)+1,0)==-1){
        perror("send");
        exit(1);
    }
    
    while(1){
        printf("Message : ");
        scanf("%s",buf);
        if(send(sd,buf,sizeof(buf)+1,0)==-1){
            perror("send");
            exit(1);
        }
    }
    close(sd);
}