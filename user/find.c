#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char*path,char*target){
    char buf[512],*p;
    int fd;
    struct dirent de;
    struct stat st;

    //打开目录
    fd=open(path,0);
    if(fd<0){
        fprintf(2, "ls: cannot open %s\n", path);
        return;
    }

    //获取目录状态
    if(fstat(fd,&st)<0){
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return;
    }

    //分类处理：文件、目录
    switch (st.type){
        //文件：比较路径的文件名（尾缀）是否与目标一致
        case T_FILE:
            if(strcmp(path+strlen(path)-strlen(target),target)==0){
                printf("%s\n",path);
            }
            break;

        //目录：递归处理
        case T_DIR:
            //路径名太长
            if(strlen(path)+1+DIRSIZ+1>sizeof(buf)){
                fprintf(2,"find: path too long\n");
                break;
            }
            //构造路径"path/"
            strcpy(buf,path);
            p=buf+strlen(buf);
            *p++='/';
            //遍历目录项
            while(read(fd,&de,sizeof(de))==sizeof(de)){//读取到的目录项信息在de中
                if(de.inum==0)continue;//空目录项

                //构造路径
                memmove(p, de.name, DIRSIZ);//安全拷贝路径名
                p[DIRSIZ]=0;//路径名结尾

                //获取目录项状态
                if(stat(buf,&st)<0){
                    fprintf(2,"find: cannot stat %s\n",buf);
                    continue;
                }

                //递归查找，排除.和..目录
                if(strcmp(buf+strlen(buf)-1,".")!=0&&strcmp(buf+strlen(buf)-2,"..")!=0){
                    find(buf,target);//递归查找子目录
                }
            }
            break;
    }
    close(fd);//关闭目录文件描述符
}

int main(int argc,char*argv[]){
    //解析得到文件名
    if(argc!=3){
        printf("Usage:find path filename\n");
        exit(1);
    }

    //得到路径和目标
    find(argv[1],argv[2]);
    exit(0);
}