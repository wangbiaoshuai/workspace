// M-eM-^OM-^QM-iM-^@M-^AM-gM-+M-/  
#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

using namespace std;

int main()
{
    setvbuf(stdout, NULL, _IONBF, 0);
    fflush(stdout);

    int sock = -1;
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        cout<<"socket error"<<endl;
        return false;
    }

    const int opt = 1;
    //M-hM-.M->M-gM-=M-.M-hM-/M-%M-eM-%M-^WM-fM-^NM-%M-eM--M-^WM-dM-8M-:M-eM-9M-?M-fM-^RM--M-gM-1M-;M-eM-^^M-^KM-oM-<M-^L  
    int nb = 0;
    nb = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt));
    if(nb == -1)
    {
        cout<<"set socket error..."<<endl;
        return false;
    }

    struct sockaddr_in addrto;
    bzero(&addrto, sizeof(struct sockaddr_in));
    addrto.sin_family=AF_INET;
    addrto.sin_addr.s_addr=htonl(INADDR_BROADCAST);
    addrto.sin_port=htons(6000);
    int nlen=sizeof(addrto);

    while(1)
    {
        sleep(1);
        //M-dM-;M-^NM-eM-9M-?M-fM-^RM--M-eM-^\M-0M-eM-^]M-^@M-eM-^OM-^QM-iM-^@M-^AM-fM-6M-^HM-fM-^AM-/  
        char smsg[] = {"abcdef"};
        int ret=sendto(sock, smsg, strlen(smsg), 0, (sockaddr*)&addrto, nlen);
        if(ret<0)
        {
            cout<<"send error...."<<ret<<endl;
        }
        else
        {      
            printf("ok ");
        }
    }

    return 0;
}
