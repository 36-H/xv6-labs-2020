#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"
#include "kernel/fs.h"

char* readLine();

int main(int argc, char *argv[]){
    if(argc < 2) {
        printf("Usage: xargs [command]\n");
        exit(-1);
    }
     
    //获取参数
    char *args_buffer[MAXARG];
    char **args = args_buffer;
    //在 char *argv[] 中 argv[0] 为执行的命令 而参数从argv[1]开始填充
    for(int i = 1; i < argc;i++){
        *args = argv[i];
        // printf("%s\n", argv[i]);
        args++;
    }
    char *line;
    char **start = args; //每读完一行要重置参数列表
    //读取一行
    while((line = readLine()) != 0){
        args = start;
        //进行参数拼接 也就是对line 进行 spilte 操作 拼接到args_buffer
        char *cur = line; //遍历指针
        char *buf = malloc(64); //单个参数缓冲区
        char *buf_ptr = buf;
        while(*cur != 0){
            if(*cur == ' ' && buf != buf_ptr){ //说明有参数且读到空格
                *buf_ptr = 0;
                *args = buf;
                args++;
                buf = malloc(64);
                buf_ptr = buf;
            }else{
                *buf_ptr = *cur;
                buf_ptr++;
            }
            cur++;
        }
        if(buf != buf_ptr){
            *args = buf;
            args++;
        }
        *args = 0;
        free(line);
        //执行
        if(fork() == 0){
            // for(char **p = args_buffer;*p != 0; p++){
            //     printf("%s\n",*p);
            // }
            exec(argv[1], args_buffer);
        }else{
            wait(0);
        }
    }
    exit(0);
}

char* readLine(){
    char* buf = malloc(512);
    char* cur = buf;
    // 我们会从标准输入流按行读取 fd = 0 为标准输出流
    // 每次读取一个字节
    while(read(0,cur,1) != 0){
        //读取结束
        if(*cur == '\0' || *cur == '\n'){
            *cur = '\0';
            return buf;
        }
        cur++;
    }
    if(cur == buf){
        free(buf);
        return 0;
    }
    return buf;
}