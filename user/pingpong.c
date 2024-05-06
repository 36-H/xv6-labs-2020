// pingpong
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(){

    // [0] 读取 [1] 写入
    int pipe1[2],pipe2[2];

    //pipe1: 父 -> 子
    //pipe2: 子 -> 父
    //          父        子
    //  read   pipe2[0]   pipe1[0]
    //  write  pipe1[1]   pipe2[1]
    pipe(pipe1);
    pipe(pipe2);

    if(fork() != 0){
        write(pipe1[1],"1",1);
        char buf;
        read(pipe2[0],&buf,1);
        printf("%d: received pong\n", getpid());
        wait(0);
    }else{
        char buf;
        read(pipe1[0],&buf,1);
        printf("%d: received ping\n", getpid());
        write(pipe2[1],"1",1);
    }
    exit(0);
}