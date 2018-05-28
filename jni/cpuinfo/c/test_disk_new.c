#include <stdio.h>
#include <string.h>

typedef struct DiskInfo
{
    char disk_name[20];
    char disk_capacity[10];
    char disk_use_rate[5];
} DiskInfo;

int main()
{
    DiskInfo array_disk_info[15] = {0};
    char line[512] = {0};
    FILE* fp = popen("df -lh -P | egrep -v 'Filesystem|文件系统' | awk '{print $6,$2,$5}'", "r");
    if(fp)
    {
        int i = 0;
        while(fgets(line, sizeof(line) - 1, fp) != NULL)
        {
            char* res = strtok(line, " ");
            if(res)
            {
                strncpy(array_disk_info[i].disk_name, res, strlen(res));
                printf("disk_name: %s", array_disk_info[i].disk_name);
            }
            
            if((res = strtok(NULL, " ")) != NULL)
            {
                strncpy(array_disk_info[i].disk_capacity, res, strlen(res));
                printf(", disk_capacity: %s", array_disk_info[i].disk_capacity);
            }

            int offset = strlen(array_disk_info[i].disk_name) + strlen(array_disk_info[i].disk_capacity) + 1 + 1;
            strcpy(array_disk_info[i].disk_use_rate, line + offset);
            printf(", disk_use_rate: %s\n", array_disk_info[i].disk_use_rate);
            i++;
        }
    }
    pclose(fp);
}
