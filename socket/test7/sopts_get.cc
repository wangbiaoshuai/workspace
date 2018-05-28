#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <linux/in.h>
#include <netinet/tcp.h>
#include <unistd.h>

typedef union optval
{
    int val;
    struct linger linger;
    struct timeval tv;
    unsigned char str[16];
}val;
static val optval;

typedef enum Valtype
{
    VALINT,
    VALLINGER,
    VALTIMEVAL,
    VALUCHAR,
    VALMAX
}Valtype;

typedef struct sopts
{
    int level;
    int optname;
    char* name;
    Valtype valtype;
}sopts;

sopts sockopts[] = {
    {SOL_SOCKET, SO_BROADCAST, "SO_BROADCAST", VALINT},
    {SOL_SOCKET, SO_DEBUG, "SO_DEBUG", VALINT},
    {SOL_SOCKET, SO_DONTROUTE, "SO_DONTROUTE", VALINT},
    {SOL_SOCKET, SO_ERROR, "SO_ERROR", VALINT},
    {SOL_SOCKET, SO_KEEPALIVE, "SO_KEEPALIVE", VALINT},
    {SOL_SOCKET, SO_LINGER, "SO_LINGER", VALINT},
    {SOL_SOCKET, SO_OOBINLINE, "SO_OOBINLINE", VALINT},
    {SOL_SOCKET, SO_RCVBUF, "SO_RCVBUF", VALINT},
    {SOL_SOCKET, SO_RCVLOWAT, "SO_RCVLOWAT", VALINT},
    {SOL_SOCKET, SO_RCVTIMEO, "SO_RCVTIMEO", VALTIMEVAL},
    {SOL_SOCKET, SO_SNDTIMEO, "SO_SNDTIMEO", VALTIMEVAL},
    {SOL_SOCKET, SO_TYPE, "SO_TYPE", VALINT},
    {IPPROTO_IP, IP_HDRINCL, "IP_HDRINCL", VALINT},
    {IPPROTO_IP, IP_OPTIONS, "IP_OPTIONS", VALINT},
    {IPPROTO_IP, IP_TOS, "IP_TOS", VALINT},
    {IPPROTO_IP, IP_TTL, "IP_TTL", VALINT},
    {IPPROTO_IP, IP_MULTICAST_TTL, "IP_MULTICAST_TTL", VALUCHAR},
    {IPPROTO_IP, IP_MULTICAST_LOOP, "IP_MULTICAST_LOOP", VALUCHAR},
    {IPPROTO_TCP, TCP_KEEPCNT, "TCP_KEEPCNT", VALINT},
    {IPPROTO_TCP, TCP_MAXSEG, "TCP_MAXSEG", VALINT},
    {IPPROTO_TCP, TCP_NODELAY, "TCP_NODELAY", VALINT},
    {0, 0, NULL, VALMAX}
};

static void disp_outcome(sopts* sockopt, int len, int err)
{
    if(err == -1)
    {
        printf("optname %s NOT support.\n", sockopt->name);
        return;
    }

    switch(sockopt->valtype)
    {
    case VALINT:
        printf("optname %s: default is %d\n", sockopt->name, optval.val);
        break;
    case VALLINGER:
        printf("optname %s: default is %d(ON/OFF), %d to linger\n", sockopt->name, optval.linger.l_onoff, optval.linger.l_linger);
        break;
    case VALTIMEVAL:
        printf("optname %s: default is %.6fms\n", sockopt->name, optval.tv.tv_sec * 1000.0 + optval.tv.tv_usec / 1000.0);
        break;
    case VALUCHAR:
        {
        printf("optname %s: default is ", sockopt->name);
        for(int i = 0; i < len; i++)
        {
            printf("%02X ", optval.str[i]);
        }
        printf("\n");
        break;
        }
    default:
        break;
    }
}

int main()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        printf("create socket error: %s\n", strerror(errno));
        return -1;
    }

    int i = 0;
    socklen_t len = sizeof(optval);
    int err;
    while(sockopts[i].valtype != VALMAX)
    {
        err = getsockopt(sock, sockopts[i].level, sockopts[i].optname, &optval, &len);
        disp_outcome(&sockopts[i], len, err);
        i++;
    }
    close(sock);
    return 0;
}
