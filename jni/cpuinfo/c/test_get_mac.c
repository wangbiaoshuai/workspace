#include <stdio.h>
#include <string.h>

int main(void)
{
    char mac[20] = {0};
    char ip[20] = {0};

    FILE* fp = popen("ifconfig | egrep 'HWaddr|硬件地址' | awk '{print $5}'", "r");
    if(fp)
    {
        fgets(mac, sizeof(mac) - 1, fp);
        mac[strlen(mac) - 1] = 0;
    }
    pclose(fp);

    fp = popen("ifconfig |grep inet |grep -v 127.0.0.1 |grep -v inet6 |awk '{print $2}' |tr -d 'addr 地址:'", "r");
    if(fp)
    {
        fgets(ip, sizeof(ip) - 1, fp);
        ip[strlen(ip) - 1] = 0;
    }
    pclose(fp);
    printf("mac: %s, ip: %s\n", mac, ip);
    return 0;
}
