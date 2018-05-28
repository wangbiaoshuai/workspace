#include <string.h>
#include <stdio.h>
int main()
{
    char cpu_arch[10] = {0};
    char cpu_rate[10] = {0};
    char cpu_processor[10] = {0};
    char line[512] = {0};
    int cpu_num = 0;
    FILE* fp = fopen("/proc/cpuinfo", "r");
    if(fp)
    {
        while(!feof(fp))
        {
            memset(line, 0, sizeof(line));
            fgets(line, sizeof(line) - 1, fp);
            if(feof(fp))
                break;
            line[strlen(line) - 1] = 0;
            switch(line[0])
            {
            case 'p':
                if(strncmp(line, "processor", 9) == 0)
                {
                    cpu_num++;
                }
                break;
            case 'c':
                if(strncmp(line, "cpu MHz", 7) == 0)
                {
                    char* res = strtok(line, " ");
                    while(res)
                    {
                        strcpy(cpu_rate, res);
                        res = strtok(NULL, " ");
                    }
                }
                break;
            default:
                break;
            }
        }
    }
    fclose(fp);

    sprintf(cpu_processor, "%d", cpu_num);

    fp = fopen("/proc/asound/oss/sndstat", "r");
    if(fp)
    {
        while(!feof(fp))
        {
            memset(line, 0, sizeof(line));
            fgets(line, sizeof(line) - 1, fp);
            if(feof(fp))
                break;
            line[strlen(line) - 1] = 0;
            switch(line[0])
            {
            case 'K':
                if(strncmp(line, "Kernel", 6) == 0)
                {
                    char* res = strtok(line, " ");
                    while(res)
                    {
                        strcpy(cpu_arch, res);
                        res = strtok(NULL, " ");
                    }
                }
                break;
            default:
                break;
            }
        }
    }
    fclose(fp);
    printf("cpu_processor: %s, cpu_rate: %s, cpu_arch: %s\n", cpu_processor, cpu_rate, cpu_arch);

    return 0;
}
