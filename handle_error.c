#include <stdio.h>
#include <stdlib.h>
#include "handle_error.h"

void handle_error(int ret, const char *msg) { // 함수 예외 처리
    if (ret == -1) {
        perror(msg);
        exit(1);
    }
}