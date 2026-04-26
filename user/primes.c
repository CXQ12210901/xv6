#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

//打印接收的第一个质数，接收一个管道作为参数,0读1写,0为管道结束标志
void print_primes(int pleft[2]){
    //读取第一个数
    int prime;
    read(pleft[0],&prime,sizeof(int));

    //结束
    if(prime==0){
        close(pleft[0]);
        exit(0);
    }

    //打印第一个数
    printf("prime %d\n",prime);

    //传递剩余的数
    int pright[2];//接收管道
    pipe(pright);

    if(fork()==0){//子进程，递归处理
        close(pright[1]);//关闭右邻居写
        close(pleft[0]);//关闭左邻居读
        print_primes(pright);
    }
    else{//父进程
        close(pright[0]);//关闭右邻居读
        
        //接收左邻居数字
        int num;
        while(read(pleft[0],&num,sizeof(int))&&num!=0){
            if(num%prime!=0){//不是第一次接收同时不是倍数，传递给右邻居，相当于滤除部分合数
                write(pright[1],&num,sizeof(int));
            }
        }

        //结束标志
        num=0;
        write(pright[1],&num,sizeof(int));

        close(pleft[0]);
        close(pright[1]);
        wait(0);//等待子进程结束
        exit(0);
    }
}

int main(int argv,char*argc[]){
    int p[2];
    pipe(p);

    if(fork()==0){
        //子进程，递归处理
        close(p[1]);//关闭写
        print_primes(p);
        exit(0);
    }
    else{
        //父进程，写入2-35
        close(p[0]);//关闭读

        //写入2-35
        for(int i=2;i<=35;i++){
            write(p[1],&i,sizeof(int));
        }

        //结束标志
        int num=0;
        write(p[1],&num,sizeof(int));
        close(p[1]);
        wait(0);//等待子进程
        exit(0);
    }
}