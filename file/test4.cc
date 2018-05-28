#include <stdio.h>
#include <string.h>
#include <errno.h>

int main()
{
    FILE* fp_in = fopen("file_in.txt", "r");
    if(fp_in == NULL)
    {
        printf("open file file_in.txt error: %s\n", strerror(errno));
        return -1;
    }

    FILE* fp_out = fopen("file_out.txt", "w");
    if(fp_out == NULL)
    {
        printf("open file file_out.txt error: %s\n", strerror(errno));
        return -1;
    }

    while(!feof(fp_in))
    {
        char ch = fgetc(fp_in);

        if(feof(fp_in))
            break;

        //rewind(fp_out);
        fputc(ch, fp_out);
    }

    fclose(fp_out);
    fclose(fp_in);
    return 0;
}
