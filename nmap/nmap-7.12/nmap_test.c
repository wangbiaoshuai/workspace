#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "nmap_di_lib.h"

int main(int argc, char *argv[])
{
	int retval = 0;
	unsigned int ipaddr;
	struct nmap_identify_s *nmap_identify = NULL;
	struct sockaddr_in   src;
	int i;
	int *portarray = NULL;
	int loop = 0;
	char *os_scan_cmd = "-n -O --script=smb-os-discovery --host-timeout 20";

	for (loop=0; loop<argc-1;loop++)
	{
		src.sin_addr.s_addr = inet_addr(argv[loop+1]);
		ipaddr = (unsigned int)src.sin_addr.s_addr;
		printf("scan ip %s\n",argv[loop+1]);
	
		retval = devscan_identify(ipaddr,os_scan_cmd,&nmap_identify);
		if (retval == 1 && nmap_identify != NULL)
		{
			portarray = (int *)nmap_identify->portlist;
			printf("port:\n");
			printf("num=%d\n",nmap_identify->portcount);
			for (i=0; i<nmap_identify->portcount; i++)
			{
				printf("%d ",portarray[i]);
			}
			printf("\n");

			if (nmap_identify->result == 1)
			{
				printf("dev=%s os=%s\n",nmap_identify->devtype, nmap_identify->ostype);
			}
			else
			{
				printf("dev and os identify fail.\n");
			}

			//nmap_identify_free(nmap_identify);
		}
		printf("the retval of identify is %d.",retval);	
	}

	return 0;
}
