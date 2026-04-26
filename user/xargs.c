#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

//运行程序
void run(char*program,char*args[]){
    if(fork()==0){
        exec(program,args);
        exit(0);
    }
    return;
}

int main(int argc,char*argv[]){
    /*
    对于"echo hello too | xargs echo bye"
    shell 看到 |，知道左边是 echo hello too，右边是 xargs echo bye
    创建管道：左边的 echo 进程的标准输出连接到管道的写入端，右边的 xargs 进程的标准输入连接到管道的读取端。
    启动 xargs 进程：shell 用参数 ["xargs", "echo", "bye"] 启动 xargs 程序。
    此时 xargs 的 argc = 3，argv[0] = "xargs"（程序名），argv[1] = "echo"，argv[2] = "bye"。
    */
   char*argsbuf[MAXARG];
   char**args=argsbuf;//指针

    //处理xargs
    //参数校验
    if(argc<2){
        fprintf(2,"Usage: xargs command [args...]\n");
        exit(1);
    }

    //将xargs的参数保存到args中
    for(int i=1;i<argc;i++){
        *args++=argv[i];
    }

    //处理标准输入
    char buf[MAXARG];
    char*p=buf,*last_p=buf;//标准输入指针，本参数，上一参数
    char**pa=args;//参数指针
    while(read(0,p,1)!=0){
        //遇到空格或者换行符，替换为结束符\0
        if(*p==' '||*p=='\n'){
            //保存是否为换行符
            int isnewline=(*p=='\n');

            *p='\0';
            
            //参数添加到argsbuf中
            *(pa++)=last_p;
            last_p=p+1;

            //遇到换行，需要执行程序
            if(isnewline){
                *pa=0;//参数结束
                run(argv[1],argsbuf);
                pa=args;//重置参数指针
            }
        }
        p++;
    }

    //最后一行没有换行符，需要执行程序
    if(pa!=args){
        *p='\0';
        *pa=0;//参数结束
        run(argv[1],argsbuf);
    }

    while(wait(0)!=-1){}//等待所有子程序
    exit(0);
}