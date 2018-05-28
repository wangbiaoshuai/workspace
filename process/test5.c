#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>

using namespace std;

std::string GetCurrentPath()
{
    char buf[255] = {0};
    getcwd(buf, sizeof(buf));
    std::string path(buf);
    return path;
}

#include <signal.h>
#include <sys/stat.h>
int init_daemon(void)  //创建守护进程
{
    int pid;
    string cur_path = GetCurrentPath();

    //1)屏蔽一些阻断信号
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    //8)处理SIGCHLD信号
    //signal(SIGCHLD, SIG_IGN);

    //2)后台运行
    pid = fork();
    if(pid > 0)
    {
        exit(0);
    }
    else if(pid < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    //3)脱离控制终端、登录会话和进程组
    setsid();

    //4)禁止进程重新打开控制终端
    /*pid = fork();
    if(pid > 0)
    {
        exit(0);
    }
    else if(pid < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }*/

    //5)关闭打开的文件描述符
    for(int i = 3; i < NOFILE; i++)
    {
        close(i);
    }

    //6)改变当前工作目录
    chdir(cur_path.c_str());
    //cout<<"current path: "<<cur_path.c_str()<<endl;
    printf("current path: %s\n", cur_path.c_str());
    fflush(NULL);

    //7)重新设置文件创建掩码
    umask(0);

    //9)重定向标准输出
    /*int fd = creat("/tmp/a.stdout", 0644);
    if(fd < 0)
    {
        perror("creat");
    }
    dup2(fd, 1);
    dup2(fd, 2);
    close(fd);*/

    return 0;
}


int main()
{
    init_daemon();
    while(true)
    {
        string cmd = "pgrep cemsmonitor";
        int status = system(cmd.c_str());
        /*printf("WIFEXITED(status)=%d, WEXITSTATUS(status)=%d\n", WIFEXITED(status), WEXITSTATUS(status));
        fflush(NULL);*/
        cout<<"WIFEXITED(status)="<<WIFEXITED(status)<<", WEXITSTATUS(status)="<<WEXITSTATUS(status)<<"."<<endl;
        if(WIFEXITED(status) != 0)
        {
            if(WEXITSTATUS(status) != 0)
            {
                /*printf("cmd exec error, exit(%d)\n", WEXITSTATUS(status));
                fflush(NULL);*/
                cout<<"cmd exec error, exit("<<WEXITSTATUS(status)<<")."<<endl;
                return -1;
            }
            else
            {
                /*printf("process cemsmonitor is alive.\n");
                fflush(NULL);*/
                cout<<"process cemsmonitor is alive."<<endl;
            }
        }
        else
        {
            /*printf("cmd exit error.\n");
            fflush(NULL);*/
            cout<<"cmd exit error."<<endl;
            return -1;
        }
        sleep(3);
    }
    return 0;
}
