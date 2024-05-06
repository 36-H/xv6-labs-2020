#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

__attribute__((noreturn))
void prime(int input_pipe[]);

int main(){
    //创建管道
    int input_pipe[2];
    pipe(input_pipe);

    //子进程
    if(fork() == 0){
        //关闭写
        close(input_pipe[1]);
        prime(input_pipe);
        exit(0);
    }else{
        //关闭读
        close(input_pipe[0]);
        int i;
        //将 2 ~ 35 的数全部写入子进程 主进程不进行筛选 
        for(i = 2; i <= 35; i++){
            write(input_pipe[1],&i,sizeof(i));
        }
        //终结符
        i = -1;
        write(input_pipe[1],&i,sizeof(i));
    }
    wait(0);
    exit(0);
}


void prime(int input_pipe[]){

    int cur;
	read(input_pipe[0], &cur, sizeof(cur)); 
	if(cur == -1) {
		exit(0);
	}
	printf("prime %d\n", cur);


    int right_pipe[2];
    pipe(right_pipe);

    //子进程
    if(fork() == 0){
        close(right_pipe[1]);
        close(input_pipe[0]);
        prime(right_pipe);
    }else{
        //关闭读
        close(right_pipe[0]);
        int buf;
        //读输入管道数据
        while(read(input_pipe[0], &buf, sizeof(buf)) && buf != -1){
            if(buf % cur != 0){
                write(right_pipe[1],&buf,sizeof(buf));
            }
        }
        //终结符
        buf = -1;
        write(right_pipe[1],&buf,sizeof(buf));
        wait(0);
        exit(0);
    }
}