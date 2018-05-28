#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int send_fd(int sock_fd, void* data, size_t bytes, int file_fd)
{
    struct msghdr msghdr_send;
    struct iovec iov[1];

    union {
        struct cmsghdr cm;
        char control[CMSG_SPACE(sizeof(int))];
    } control_un;

    struct cmsghdr* pcmsghdr = NULL;


}

int main(int argc, char* argv[])
{
    if(argc != 4)
    {
        printf("usage error.\n");
        return -1;
    }

    char* file_name = argv[2];
    char* open_mode = argv[3];

    int socket_fd = atoi(argv[1]);

    int fd = open(file_name, open_mode);
    if(fd < 0)
    {
        printf("open %s error: %s\n", file_name, strerror(errno));
        return -1;
    }

    int res = send_fd(socket_fd, "", 0, fd);
    if(res < 0)
    {
        printf("send_fd failed.\n");
        return -1;
    }

    return 0;
}
