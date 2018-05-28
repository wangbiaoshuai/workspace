#include <sys/time.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

int main()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    int64_t current_time = time.tv_sec * 1000 + time.tv_usec / 1000;
    printf("time: %ld\n", current_time);

    time_t timer = current_time / 1000;
    char tmBuf[100] = {0};
    strftime(tmBuf, 100, "%Y-%m-%d %H:%M:%S", localtime(&timer));
    printf("%s\n", tmBuf);

    const char* name = "\u4e2d\u8f6c\u670d\u52a1";
    printf("name: %s\n", name);
    return 0;
}
