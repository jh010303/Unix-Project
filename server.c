#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PORTNUM 9005

struct user_info{
    char username[20];
};

// TCP 서버 
int main(){
    char buf[BUFSIZ];
    struct sockaddr_in sin,cli1,cli2;
    int sd,cd1,cd2,clientlen = sizeof(cli1);
    
    FILE* fs;
    fs = fopen("ip_address.txt", "r");
    char str[BUFSIZ];
    fgets(str,BUFSIZ,fs);

    // 서버 소켓
    if((sd=socket(AF_INET,SOCK_STREAM,0))==-1){ // TCP, 인터넷 소켓
        perror("socket");
        exit(1);
    }

    memset((char*)&sin,'\0',sizeof(sin)); // 소켓 주소 구조체 초기화

    sin.sin_family = AF_INET; // 소켓 주소 구조체에 유닉스 도메인 소켓임을 알림
    sin.sin_port = htons(PORTNUM);
    sin.sin_addr.s_addr = inet_addr(str);   

    if(bind(sd,(struct sockaddr*)&sin,sizeof(sin))){ // bind를 통해 소켓 기술자(sd)와 소켓 주소 구조체 연결
        perror("bind");
        exit(1);
    }

    // 리스닝
    if(listen(sd,3)){
        perror("listen");
        exit(1);
    }

    // client 1 연결됨
    if((cd1 = accept(sd,(struct sockaddr*)&cli1, &clientlen))==-1){
        perror("accept client1");
        exit(1);
    }

    printf("client1 connected\n");

    // client1의 username 받음
    if(recv(cd1,buf,sizeof(buf),0)==-1){
        perror("recv");
        exit(1);
    }

    printf("%s connected\n",buf);

    // client 2 연결됨
    if((cd2 = accept(sd,(struct sockaddr*)&cli2, &clientlen))==-1){
        perror("accept client2");
        exit(1);
    }

    printf("client2 connected\n");

    // client2의 username 받음
    if(recv(cd2,buf,sizeof(buf),0)==-1){
        perror("recv");
        exit(1);
    }

    printf("%s connected\n",buf);

    send(cd1,&cli2,sizeof(cli2),0);
    send(cd2,&cli1,sizeof(cli1),0);

    printf("Client1 and Client2 is interconnected\n");
    close(cd1);
    close(cd2);
    close(sd);

    return 0;
}