#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc,char*argv[]){
    //创建管道,0读,1写
    int pp2c[2],pc2p[2];//管道，父->子，子->父
    pipe(pp2c);
    pipe(pc2p);

    //创建进程，fork()返回0表示子进程，返回其他表示父进程
    if(fork()==0){//子，先读后写
        //读
        char buf[4];
        read(pp2c[0],buf,4);
        printf("%d: received ping\n",getpid());

        //写，写后关闭
        write(pc2p[1],"pong",4);
        close(pc2p[1]);

        //等待子进程结束
        wait(0);
    }
    else{//父，先写后读
        //写
        write(pp2c[1],"ping",4);
        close(pp2c[1]);

        //读
        char buf[4];
        read(pc2p[0],buf,4);
        printf("%d: received pong\n",getpid());

        //等待父进程结束
        // wait(getpid());
    }
    //关闭读通道
    close(pp2c[0]);
    close(pc2p[0]);
    exit(0);
}