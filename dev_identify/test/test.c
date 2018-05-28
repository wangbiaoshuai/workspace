#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <dev_identify.h>

static inline void incomplete_command()
{
    fprintf(stderr, "Command line is not complete!\n");
    exit(-1);
}

#define next_arg()  do{ argv++; if (--argc <= 0) incomplete_command(); }while(0)

static int match(char const *src, char *dst)
{
    int len = strlen(src);
	
	if (len != strlen(dst))
		return -1;

	return memcmp(dst, src, len);
}

int main(int argc, char **argv)
{
	unsigned int ipaddr = 0;
	struct devscan_result_s devscan_result;
	char buf[512];
	int i = 0;
	int len = 0;

	memset(&devscan_result,0,sizeof(devscan_result));

	argc--; argv++;
	while (argc >= 1)
    {

	    if (match(*argv, "addr") == 0)
	    {
	        next_arg();
            ipaddr = inet_addr(*argv);
	    }
	    else
	    	goto out;
	        
        argc--; argv++;
	}

	devscan_identify(ipaddr,&devscan_result);

	if (devscan_result.status == 0)
	{
		printf("The ip is unreachable.\n");
	}
	else
	{
		printf("devtype_id=%d\n",devscan_result.devtype_id);
		printf("devtype_name=%s\n",devscan_result.devtype_en_name);
		printf("devclass_name=%s\n",devscan_result.devclass_name);
		printf("sysclass_id=%d\n",devscan_result.sysclass_id);
		printf("sysclass_name=%s\n",devscan_result.sysclass_name);
		printf("systype_name=%s\n",devscan_result.systype_name);
		printf("portcount=%d\n",devscan_result.portcount);

		memset(buf,0,sizeof(buf));
		for (i=0; i<devscan_result.portcount; i++)
		{
			len +=sprintf(buf+len,"%s%d",i==0?"":",",devscan_result.portlist[i]);
		}
		printf("portlist=%s\n",buf);
	}	

	return 0;

out:
	printf("arg error");

	return 0;
}

