#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main()
{
    int pid = fork();
    if(pid == 0)
    {
        sleep(6);
        printf("I am child.\n");
        exit(0);
    }
    
    signal(SIGCHLD, SIG_IGN);
    int status;
    //wait(&status);
    sleep(15);
    return 0;
}
