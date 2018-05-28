#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/if_ether.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

#define DEST_ADDR "192.168.133.93"

struct arppacket
{
    unsigned short ar_hrd;
    unsigned short ar_pro;
    unsigned char ar_hln;
    unsigned char ar_pln;
    unsigned short ar_op;
    unsigned char ar_sha[ETH_ALEN];
    uint32_t ar_sip;
    unsigned char ar_tha[ETH_ALEN];
    uint32_t ar_tip;
};

int main()
{
    printf("unsigned long:%d, unsigned short:%d\n", sizeof(unsigned long), sizeof(unsigned short));
    char ef[ETH_FRAME_LEN] = {0};

    int sock = socket(AF_INET, SOCK_PACKET, htons(0x0806));
    if(sock < 0)
    {
        printf("create socket error: %s\n", strerror(errno));
        return -1;
    }
    struct arppacket* p_arppacket;
    struct ethhdr* p_ethhdr;

    uint32_t dest_ip = inet_addr(DEST_ADDR);

    while(true)
    {
        memset(ef, 0, sizeof(ef));
        int n = read(sock, ef, sizeof(ef));
        if(n <= 0)
        {
            printf("read error: %s\n", strerror(errno));
            return -1;
        }

        //解析eth头
        p_ethhdr = (struct ethhdr*)ef;
        if(p_ethhdr->h_proto != htons(0x0806))
        {
            printf("not arp.\n");
            continue;
        }

        //解析arp包
        p_arppacket = (struct arppacket*)(ef + 14);

        /*if(p_arppacket->ar_tip != dest_ip)
        {
            printf("not dest ip.\n");
            continue;
        }*/

        printf("source mac: ");
        for(int i = 0; i < 5; i++)
        {
            printf("%02x:", p_arppacket->ar_sha[i]);
        }
        printf("%02x\n", p_arppacket->ar_sha[5]);
        struct in_addr addr;
        addr.s_addr = p_arppacket->ar_sip;
        printf("source ip: %s\n", inet_ntoa(addr));

        printf("dest mac: ");
        for(int i = 0; i < 5; i++)
        {
            printf("%02x:", p_arppacket->ar_tha[i]);
        }
        printf("%02x\n", p_arppacket->ar_tha[5]);
        addr.s_addr = p_arppacket->ar_tip;
        printf("dest ip: %s\n", inet_ntoa(addr));

        //break;
    }

    close(sock);
    return 0;
}
