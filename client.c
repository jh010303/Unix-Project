#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define PORTNUM 9005

int sd;
char name[20],yourname[20];

void *receive_messages(void *arg) {
    char buf[BUFSIZ];
    while (1) {
        sleep(0.1);
        memset(buf, 0, BUFSIZ);
        if(recv(sd,buf,sizeof(buf),0)==-1){
            perror("recv");
            exit(1);
        }
        printf("%s",buf);
    }
    close(sd);
    exit(0);
}

void *send_messages(void *arg) {
    char buf[BUFSIZ];
    char message[8000];
    while (1) {
        sleep(0.1);
        memset(buf, 0, BUFSIZ);
        memset(message, 0, BUFSIZ);
        fgets(message, BUFSIZ, stdin);
        snprintf(buf,sizeof(buf),"%s: %s",yourname,message);
        if(send(sd,buf,sizeof(buf)+1,0)==-1){
            perror("send");
            exit(1);
        }
    }
}

// TCP client 
int main(){
    char buf[BUFSIZ],command[10];
    struct sockaddr_in sin,cli;
    int n,len = sizeof(sin);
    pthread_t send_thread, recv_thread;

    FILE* fs;
    fs = fopen("ip_address.txt", "r");
    char str[BUFSIZ];
    fgets(str,BUFSIZ,fs); // str에는 ip주소 들어있음

    // 인터넷 소켓, TCP 사용
    if((sd=socket(AF_INET,SOCK_STREAM,0))==-1){ // client 소캣
        perror("socket");
        exit(1);
    }

    memset((char*)&sin,'\0',sizeof(sin)); // 소켓 주소 구조체 초기화
    memset((char*)&cli,'\0',sizeof(cli)); // 다른 클라이언트 주소 구조체 초기화

    sin.sin_family = AF_INET; // 소켓 주소 구조체에 인터넷 소켓임을 알림
    sin.sin_port = htons(PORTNUM); // 포트번호
    sin.sin_addr.s_addr = inet_addr(str); //ip주소를 숫자로 변환

    // 서버에 연결
    if(connect(sd,(struct sockaddr*)&sin,sizeof(sin))){
        perror("connect");
        exit(1);
    }

    printf("server connected\n");

    // 닉네임 입력
    printf("nickname: ");
    scanf("%s",yourname);
    
    // 서버에게 닉네임 전달
    if(send(sd,yourname,sizeof(yourname)+1,0)==-1){
        perror("send");
        exit(1);
    }

    // 서버에게 다른 클라이언트 정보 받음
    if(recv(sd,&cli,sizeof(cli),0)==-1){
        perror("recv");
        exit(1);
    }

    // 서버에게 다른 클라이언트 닉네임 받음
    if(recv(sd,&name,sizeof(name),0)==-1){
        perror("recv");
        exit(1);
    }

    // 서버에게 명령어 정보 받음
    if(recv(sd,&command,sizeof(command),0)==-1){
        perror("recv");
        exit(1);
    }

    close(sd);
    sleep(1);
    
    if((sd=socket(AF_INET,SOCK_STREAM,0))==-1){ // client 소캣
        perror("socket");
        exit(1);
    }

    memset((char*)&sin,'\0',sizeof(sin)); // 소켓 주소 구조체 초기화

    sin.sin_family = AF_INET; // 소켓 주소 구조체에 인터넷 소켓임을 알림
    sin.sin_port = htons(PORTNUM); // 포트번호
    sin.sin_addr.s_addr = inet_addr(str); //ip주소를 숫자로 변환

    if(strcmp(command,"CONNECT")==0){ // 다른 클라이언트와 연결 ( connect ), client 역할
        sleep(0.5);
        if(connect(sd,(struct sockaddr*)&sin,sizeof(sin))==-1){
            perror("connect");
            close(sd);
            exit(1);
        }
    }
    else{    // 다른 클라이언트와 연결 (listen,accept) -> server 역할
        if(bind(sd,(struct sockaddr*)&sin,sizeof(sin))){ // bind를 통해 소켓 기술자(sd)와 소켓 주소 구조체 연결
            perror("bind");
            exit(1);
        }   
       if(listen(sd,2)==-1){
            perror("listen");
            close(sd);
            exit(1);
        }
        printf("Waiting user ...\n");
        if((sd = accept(sd,(struct sockaddr*)&cli,&len))==-1){
            perror("accept client2");
            exit(1);
        }
    }

    memset(buf, 0, BUFSIZ);
    printf("%s has joined your chat!\n",name);
    printf("================ Chat Room ================\n");
    pthread_create(&send_thread, NULL, send_messages, NULL);
    pthread_create(&recv_thread, NULL, receive_messages, NULL);

    pthread_join(send_thread, NULL);
    pthread_join(recv_thread, NULL); 

    close(sd);
}