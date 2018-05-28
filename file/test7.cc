#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <stdlib.h>
using namespace std;

int main()
{
    int num = 125;
    char name[] = "wangbiaoshuai";
    double cost = 356.22;

    char buf[8 + 50 + 20] = {0};
    sprintf(buf, "%d,%s,%.2lf", num, name, cost);
    //fputs(buf, stdout);

    FILE* file_out = fopen("student", "w+");
    if(file_out == NULL)
    {
        printf("open file student error: %s\n", strerror(errno));
        return -1;
    }

    //fwrite(buf, strlen(buf), 1, file_out);
    fputs(buf, file_out);
    fputs("\r\n", file_out);
    fputs(buf, file_out);
    fputs("\r\n", file_out);

    rewind(file_out);
    char out_buf[8 + 50 + 20] = {0};

    char num_out[8] = {0};
    char name_out[50] = {0};
    char cost_out[20] = {0};

    //fscanf(file_out, "%d, %s, %f", &num_out, name_out, &cost_out);
    while(!feof(file_out))
    {
        fgets(out_buf, sizeof(out_buf), file_out);
        if(feof(file_out))
            break;

        sscanf(out_buf, "%[^,],%[^,],%[1-9,.]", num_out, name_out, cost_out);
        printf("%s\n", cost_out);

        printf("%d, %s, %.2lf\n", atoi(num_out), name_out, atof(cost_out));
    }
    fclose(file_out);

    double x = 512.34;
    char swap[10] = {0};
    sprintf(swap, "%lf", x);
    printf("swap: %s\n", swap);
    double m = 0.0;
    m = atof(swap);
    //sscanf(swap, "%f", &m);
    printf("m: %.2lf\n", m);
    return 0;
}
