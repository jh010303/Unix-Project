#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "init_socket.h"
#include "handle_error.h"

#define PORTNUM 9005

// TCP 서버 
int main(){
    char buf[BUFSIZ],name1[20],name2[20];
    struct sockaddr_in sin,cli1,cli2;
    int sd,cd1,cd2,clientlen = sizeof(cli1);
    
    FILE* fs;
    fs = fopen("ip_address.txt", "r");
    char str[BUFSIZ];
    fgets(str,BUFSIZ,fs);

    // 서버 소켓
    init_socket(&sd,str,PORTNUM,&sin); // 소켓 초기화

    handle_error(bind(sd,(struct sockaddr*)&sin,sizeof(sin)),"bind"); // bind를 통해 소켓 기술자(sd)와 소켓 주소 구조체 연결
    handle_error(listen(sd,3),"listen"); // 리스닝
    handle_error((cd1 = accept(sd,(struct sockaddr*)&cli1, &clientlen)),"accept"); // client 1 연결됨
    printf("client1 connected\n");

    // client1의 username 받음
    handle_error(recv(cd1,name1,sizeof(name1),0),"recv");
    printf("%s connected\n",name1);

    // client 2 연결됨
    handle_error((cd2 = accept(sd,(struct sockaddr*)&cli2, &clientlen)),"accept");
    printf("client2 connected\n");

    // client2의 username 받음
    handle_error(recv(cd2,name2,sizeof(name2),0)==-1,"recv");
    printf("%s connected\n",name2);

    send(cd1,&cli2,sizeof(cli2),0); // client1에게 client2 소캣 구조체 전송
    send(cd1,&name2,sizeof(name2),0); // client1에게 client2 닉네임 전송
    sleep(0.2);
    send(cd1, "LISTEN", 7, 0); // client1은 server 역할
    sleep(0.5);
    send(cd2,&cli1,sizeof(cli1),0); 
    send(cd2,&name1,sizeof(name1),0);
    sleep(0.2);
    send(cd2, "CONNECT", 8, 0); // client2는 client 역할
    printf("Client1 and Client2 is interconnected\n");
    close(cd1);
    close(cd2);
    close(sd);
    return 0;
}