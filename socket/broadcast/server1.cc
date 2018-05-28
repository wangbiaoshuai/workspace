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

    struct sockaddr_in addrto;  
    bzero(&addrto, sizeof(struct sockaddr_in));  
    addrto.sin_family = AF_INET;  
    addrto.sin_addr.s_addr = htonl(INADDR_ANY);  
    addrto.sin_port = htons(6000);  

    struct sockaddr_in from;  
    bzero(&from, sizeof(struct sockaddr_in));  
    from.sin_family = AF_INET;  
    from.sin_addr.s_addr = htonl(INADDR_ANY);  
    from.sin_port = htons(6000);  

    int sock = -1;  
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)   
    {     
        cout<<"socket error"<<endl;   
        return false;  
    }     

    const int opt = 1;  
    int nb = 1;  
    nb = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt));  
    if(nb == -1)  
    {  
        cout<<"set socket error..."<<endl;  
        return false;  
    }  

    if(bind(sock,(struct sockaddr *)&(addrto), sizeof(struct sockaddr_in)) == -1)   
    {     
        cout<<"bind error..."<<endl;  
        return false;  
    }  

    int len = sizeof(sockaddr_in);  
    char smsg[100] = {0};  

    while(1)  
    {  
        int ret=recvfrom(sock, smsg, 100, 0, (struct sockaddr*)&from,(socklen_t*)&len);  
        if(ret<=0)  
        {  
            cout<<"read error...."<<sock<<endl;  
        }  
        else  
        {         
            printf("%s\t", smsg);     
        }  

        sleep(1);  
    }  

    return 0;  
}
