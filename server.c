#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PORTNUM 9005

// UDP 동시 동작 서버 
int main(){
    char buf[BUFSIZ];
    struct sockaddr_in sin,cli;
    int sd,ns,clientlen = sizeof(cli);
    
    // 인터넷 소켓, UDP 사용
    if((sd=socket(AF_INET,SOCK_STREAM,0))==-1){ // 서버측 udp 소켓
        perror("socket");
        exit(1);
    }

    memset((char*)&sin,'\0',sizeof(sin)); // 소켓 주소 구조체 초기화

    sin.sin_family = AF_INET; // 소켓 주소 구조체에 유닉스 도메인 소켓임을 알림
    sin.sin_port = htons(PORTNUM);
    sin.sin_addr.s_addr = inet_addr("192.168.56.1");   

    if(bind(sd,(struct sockaddr*)&sin,sizeof(sin))){ // bind를 통해 소켓 기술자(sd)와 소켓 주소 구조체 연결
        perror("bind");
        exit(1);
    }

    if(listen(sd,3)){
        perror("listen");
        exit(1);
    }
    // 동시 동작 서버 실행
    while(1){
        if((ns = accept(sd,(struct sockaddr*)&cli,&clientlen))==-1){
            perror("accept");
            exit(1);
        }

        switch(fork()){
            case 0:
                strcpy(buf,"Welcome to Server. What's your name?");
                if(send(ns,buf,strlen(buf)+1,0)==-1){
                    perror("send");
                    exit(1);
                }

                if(recv(ns,buf,sizeof(buf),0)==-1){
                    perror("recv");
                    exit(1);
                }

                printf("From Client:%s\n",buf);
                while(1){
                    sleep(1);
                    if(send(ns,buf,strlen(buf)+1,0)==-1){
                        perror("send");
                        exit(1);
                    }
                    if(recv(ns,buf,sizeof(buf),0)==-1){
                        perror("recv");
                        exit(1);
                    }
                    printf("Message : %s\n",buf);
                }
                close(ns);
                sleep(1);
                exit(0);
        }
        // if((recvfrom(sd,buf,BUFSIZ,0,(struct sockaddr*)&cli, &clientlen))==-1){ // client가 server에게 쏜 문자 받음, buf에 저장
        //     perror("recvfrom");
        //     exit(1);
        // }

        // printf("From Client : %s\n",buf);
        // strcpy(buf,"Hello Client.");
    
        // // cli에는 수신자의 주소. 
        // if(sendto(sd,buf,strlen(buf)+1,0,(struct sockaddr*)&cli, sizeof(cli))==-1){ // client가 server에게 메세지 전송
        //     perror("sendto");
        //     exit(1);
        // }
    }
    close(sd);
}