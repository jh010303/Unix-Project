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
    char buf[BUFSIZ];
    struct sockaddr_in sin;
    int sd, n;
    
    // 인터넷 소켓, UDP 사용
    if((sd=socket(AF_INET,SOCK_DGRAM,0))==-1){ // 서버측 udp 소켓
        perror("socket");
        exit(1);
    }

    memset((char*)&sin,'\0',sizeof(sin)); // 소켓 주소 구조체 초기화

    sin.sin_family = AF_INET; // 소켓 주소 구조체에 유닉스 도메인 소켓임을 알림
    sin.sin_port = htons(PORTNUM);
    sin.sin_addr.s_addr = inet_addr("192.168.56.1");   

    strcpy(buf,"I am a client.");
    if(sendto(sd,buf,strlen(buf)+1,0,(struct sockaddr*)&sin, sizeof(sin))==-1){
        perror("sendto");
        exit(1);
    }

    n = recvfrom(sd,buf,sizeof(buf),0,NULL,NULL);
    buf[n] = '\0';
    printf("From Server : %s\n",buf);
    
}