#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <string>
using namespace std;

const char* f_cmdline = "cmdline";
const char* proc_name = "gvfsd";
bool is_exit = false;

int GrepFile(const char* file_name, const char* str)
{
    FILE* fp = fopen(file_name, "r");
    if(fp == NULL)
    {
        printf("open file error: %s\n", strerror(errno));
        return -1;
    }

    while(!feof(fp))
    {
        char line[1024] = {0};
        fgets(line, sizeof(line), fp);
        /*if(feof(fp))
        {
            break;
        }*/
        string tmp = line;
        if(tmp.find(str) != string::npos)
        {
            fclose(fp);
            return 0;
        }
    }
    fclose(fp);
    return -1;
}

int printdir(char *dir, int depth)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;

    if ((dp = opendir(dir)) == NULL) 
    {
        fprintf(stderr, "Can`t open directory %s\n", dir);
        return -1;
    }
    
    chdir(dir);
    while ((entry = readdir(dp)) != NULL) 
    {
        lstat(entry->d_name, &statbuf);
        if (S_ISDIR(statbuf.st_mode)) 
        {
            if (strcmp(entry->d_name, ".") == 0 || 
                strcmp(entry->d_name, "..") == 0 )  
                continue;   
            //printf("%*s%s/\n", depth, "", entry->d_name);
            if(printdir(entry->d_name, depth+4) == 0)
            {
                chdir("..");
                closedir(dp);
                return 0;
            }
        } 
        else
        {
            //printf("d_name: %s\n", entry->d_name);
            if(strcmp(f_cmdline, entry->d_name) == 0)
            {
                if(GrepFile(f_cmdline, proc_name) == 0)
                {
                    chdir("..");
                    closedir(dp);
                    return 0;
                }
            }
            //printf("%*s%s\n", depth, "", entry->d_name);
        }
    }
    chdir("..");
    closedir(dp);
    return -1;
}


int main(int argc, char *argv[])
{
    char *topdir = ".";
    if (argc >= 2)
        topdir = argv[1];

    printf("Directory scan of %s\n", topdir);
    if(printdir(topdir, 0) == 0)
    {
        printf("proc %s is exit\n", proc_name);
    }
    else
    {
        printf("proc %s is not exit.\n", proc_name);
    }
    printf("done.\n");
    exit(0);
}
