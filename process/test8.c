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
const char* proc_name = NULL;
#define PROC_DIR "/proc"

std::string GetCurrentPath()
{
    char buf[255] = {0};
    getcwd(buf, sizeof(buf));
    std::string path(buf);
    return path;
}

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

int ProcIsExist(const std::string& proc)
{
    string curr_path = GetCurrentPath();
    proc_name = proc.c_str();
    if(printdir(PROC_DIR, 0) == 0)
    {
        chdir(curr_path.c_str());
        return 0;
    }
    chdir(curr_path.c_str());
    return -1;
}

int main(int args, char* argv[])
{
    if(args != 2)
    {
        printf("Usage: ./a.out proc_name\n");
        return -1;
    }

    const char* proc_name = argv[1];
    if(ProcIsExist(proc_name) == 0)
    {
        printf("%s is exited!\n", proc_name);
    }
    else
    {
        printf("%s is not exited!\n", proc_name);
    }

    return 0;
}
