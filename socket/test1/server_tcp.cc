#include <sys/socket.h>
#include <wait.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#define SERVER_PORT 8087

int add_int(int x, int y)
{
    return (x + y);
}

double add_double(double x, double y)
{
    return (x + y);
}

int sock = 0;

void clean(int)
{
    printf("recieve SIG_INT.\n");
    close(sock);
}

void sig_chld(int signo) 
{ 
    pid_t   pid; 
    int     stat; 

    while((pid = waitpid(-1, &stat, WNOHANG)) > 0);
/*    { 
        printf("child %d terminated\n", pid); 
    } */
    return; 
} 

int main()
{
   // signal(SIGINT, clean);
    signal(SIGCHLD, sig_chld);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        printf("create sock errno: %s\n", strerror(errno));
        return -1;
    }
    
    struct sockaddr_in my_addr;
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    my_addr.sin_port = htons(SERVER_PORT);

    int res = bind(sock, (struct sockaddr*)&my_addr, sizeof(struct sockaddr));
    if(res < 0)
    {
        printf("bind error: %s\n", strerror(errno));
        return -1;
    }

    res = listen(sock, 10);
    if(res < 0)
    {
        printf("listen error: %s\n", strerror(errno));
        return -1;
    }

    while(1)
    {
        struct sockaddr_in client_addr;
        int addr_len = sizeof(struct sockaddr);
        int client_sock = accept(sock, (struct sockaddr*)&client_addr, (socklen_t*)&addr_len);
        if(client_sock < 0)
        {
            printf("accept errno: %s\n", strerror(errno));
            continue;
        }
        printf("server: got connection form %s\n", inet_ntoa(client_addr.sin_addr));

        int pid = fork();
        if(pid == 0)
        {
            char msg[512] = {0};
            res = recv(client_sock, msg, sizeof(msg), 0);
            if(res < 0)
            {
                printf("recv error: %s\n", strerror(errno));
                exit(1);
            }

            char fun[20] = {0};
            sscanf(msg, "%[^:]", fun);
            printf("msg: %s, fun: %s\n", msg, fun);
            if(strcmp(fun, "add_int") == 0)
            {
                char arg1[10] = {0};
                char arg2[10] = {0};
                sscanf(msg, "%[^:]:%[^:]:%[0-9]", fun, arg1, arg2);
                printf("arg1=%s, arg2=%s\n", arg1, arg2);
                res = add_int(atoi(arg1), atoi(arg2));
                char ret[10] = {0};
                sprintf(ret, "%d", res);
                res = send(client_sock, ret, strlen(ret), 0);
                if(res < 0)
                {
                    printf("send errno: %s\n", strerror(errno));
                    exit(1);
                }
            }
            else if(strcmp(fun, "add_double") == 0)
            {
                char arg1[20] = {0};
                char arg2[20] = {0};
                sscanf(msg, "%[^:]:%[^:]:%[0-9,.]", fun, arg1, arg2);
                printf("arg1=%s, arg2=%s\n", arg1, arg2);
                double result = add_double(atof(arg1), atof(arg2));
                char ret[20] = {0};
                sprintf(ret, "%lf", result);
                res = send(client_sock, ret, strlen(ret), 0);
                printf("ret=%s\n", ret);
                if(res < 0)
                {
                    printf("send errno: %s\n", strerror(errno));
                    exit(1);
                }
            }
            close(client_sock);
            exit(0);
        }
        //wait(NULL);
    }

    close(sock);
    return 0;
}
