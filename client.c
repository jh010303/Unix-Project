#include <sys/socket.h>
#include <netinet/in.h> 
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <unistd.h>
#include <pthread.h>
#include "init_socket.h"
#include "handle_error.h"

#define PORTNUM 9005

int sd; // 소켓
char name[20],yourname[20]; // 상대 닉네임, 내 닉넨임
struct sockaddr_in sinn,cli; // 소켓 구조체
volatile int is_running = 1; // thread 실행 함수 상태

void *receive_messages(void *arg) { // 상대방의 채팅 받는 함수
    char buf[BUFSIZ];
    recv(sd,buf,sizeof(buf),0);
    while (is_running) {
        sleep(0.1);
        memset(buf, '\0', BUFSIZ);
        if(recv(sd,buf,sizeof(buf),0)==-1){ // 메시지 받음
            perror("recv");
            exit(1);
        }
        if(strncmp(buf,"exit",4)==0){ // 채팅 끝내는 명령어
            printf("%s left the chat.\n",name);
            is_running = 0;
            break;
        }
        if(is_running==0){
            break;
        }
        printf("                                                %s",buf); // 받은 메시지 출력

    }
    send(sd,"!",1,0);
    return NULL;
}

void *send_messages(void *arg) { // 상대방에게 채팅 보내는 함수
    char buf[BUFSIZ];
    char message[8000];
    while (is_running) {
        sleep(0.1);
        memset(buf, '\0', BUFSIZ);
        memset(message, '\0', 8000);
        fgets(message, 8000, stdin);
        if(strncmp(message,"exit",4)==0){ // 채팅 끝내는 명령어 
            is_running = 0;
            if(send(sd,message,sizeof(message),0)==-1){
                perror("send");
                exit(1);
            }
            break;
        }
        snprintf(buf,sizeof(buf),"%s: %s",yourname,message); // 어떤 user가 보냈는지 + 메시지 내용
        if(send(sd,buf,sizeof(buf),0)==-1){
            perror("send");
            exit(1);
        }
    }
    send(sd,buf,sizeof(buf),0);
    return NULL;
}

// TCP client 
int main(){
    char command[10],ip[100]; // client가 나중에 server 역할을 할지, client 역할을 그대로 할지 구분하는 command 문자열
    int len = sizeof(sinn);
    pthread_t send_thread, recv_thread;
    // ip 주소 입력 ( ip_address.txt 사용 )
    FILE* fs;
    fs = fopen("ip_address.txt", "r");
    fgets(ip,100,fs); // str에는 ip주소 들어있음

    memset((char*)&cli,'\0',sizeof(cli)); // 다른 클라이언트 주소 구조체 초기화
    init_socket(&sd,ip,PORTNUM,&sinn); // 소켓 초기화

    // 서버에 연결
    handle_error(connect(sd,(struct sockaddr*)&sinn,sizeof(sinn)),"send");    
    printf("server connected\n");
    // 닉네임 입력
    printf("nickname: ");
    scanf("%s",yourname);
    
    handle_error(send(sd,yourname,sizeof(yourname)+1,0),"send"); // 서버에게 닉네임 전달
    handle_error(recv(sd,&cli,sizeof(cli),0),"recv"); // 서버에게 다른 클라이언트 정보 받음
    handle_error(recv(sd,&name,sizeof(name),0),"recv"); // 서버에게 다른 클라이언트 닉네임 받음
    handle_error(recv(sd,&command,sizeof(command),0),"recv"); // 서버에게 명령어 정보 받음
    
    close(sd);
    sleep(1); // Address already in use 오류를 피하기 위해

    init_socket(&sd,ip,PORTNUM,&sinn);
    if(strcmp(command,"CONNECT")==0){ // 다른 클라이언트와 연결 ( connect ), client 역할
        sleep(0.5);
        handle_error(connect(sd,(struct sockaddr*)&sinn,sizeof(sinn)),"connect");
    }
    else{    // 다른 클라이언트와 연결 (listen,accept) -> server 역할
        handle_error(bind(sd,(struct sockaddr*)&sinn,sizeof(sinn)),"bind"); // bind를 통해 소켓 기술자(sd)와 소켓 주소 구조체 연결
        handle_error(listen(sd,2),"listen");
        printf("Waiting user ...\n");
        handle_error((sd = accept(sd,(struct sockaddr*)&cli,&len)),"accept");
    }

    printf("%s has joined your chat!\n",name);
    printf("=============================== Chat Room ===============================\n");
    printf("NOTIFICATION: Enter ""exit"" if you want to leave the chat\n");
    // thread 실행
    pthread_create(&send_thread, NULL, send_messages, NULL);
    pthread_create(&recv_thread, NULL, receive_messages, NULL);
    pthread_join(send_thread, NULL);
    pthread_join(recv_thread, NULL); 
    close(sd);
}