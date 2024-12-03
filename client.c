#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PORTNUM 9005

// TCP client 
int main(){
    char buf[BUFSIZ],name[20];
    struct sockaddr_in sin,cli;
    int sd, sd2, n,len = sizeof(sin);
    
    FILE* fs;
    fs = fopen("ip_address.txt", "r");
    char str[BUFSIZ];
    fgets(str,BUFSIZ,fs);

    // 인터넷 소켓, TCP 사용
    if((sd=socket(AF_INET,SOCK_STREAM,0))==-1){ // client 소캣
        perror("socket");
        exit(1);
    }

    memset((char*)&sin,'\0',sizeof(sin)); // 소켓 주소 구조체 초기화

    sin.sin_family = AF_INET; // 소켓 주소 구조체에 유닉스 도메인 소켓임을 알림
    sin.sin_port = htons(PORTNUM);
    sin.sin_addr.s_addr = inet_addr(str);   

    // 서버에 연결
    if(connect(sd,(struct sockaddr*)&sin,sizeof(sin))){
        perror("connect");
        exit(1);
    }

    printf("server connected\n");

    printf("%s\n",buf);

    // 닉네임 입력
    printf("nickname: ");
    scanf("%s",name);
    strcpy(buf,name);

    // 서버에게 닉네임 전달
    if(send(sd,buf,sizeof(buf)+1,0)==-1){
        perror("send");
        exit(1);
    }

    // 서버에게 다른 클라이언트 정보 받음
    if(recv(sd,&cli,sizeof(cli),0)==-1){
        perror("recv");
        exit(1);
    }

    printf("%d\n",ntohs(cli.sin_port));
    printf("%s\n",inet_ntoa(cli.sin_addr));
    
    // 새로운 소켓을 통해 다른 클라이언트에 연결    
    if((sd2 = socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("socket");
        close(sd);
        exit(1);
    }

    if(connect(sd2,(struct sockaddr*)&cli,sizeof(cli))==-1){
        perror("connect");
        close(sd);
        close(sd2);
        exit(1);
    }

    while(1){
        scanf("%s",buf);
        send(sd2,buf,strlen(buf)+1,0);
        recv(sd2,buf,BUFSIZ,0);
        printf("Message: %s\n",buf);
    }

    close(sd);
    close(sd2);
}