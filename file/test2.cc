#include <stdio.h>
#include <errno.h>
#include <string.h>

typedef struct Student
{
    int num;
    char name[50];
    double cost;
    int age;
    
    Student()
    {
        num = 0;
        memset(name, 0, sizeof(name));
        age = 0;
        cost = 0.0;
    }
} Student;

int main(void)
{
    Student stu1, stu2;

    stu1.num = 1;
    memcpy(stu1.name, "wangbiaoshuai", sizeof(char) * 13);
    stu1.age = 27;
    //stu1.cost = 5120.45;
    stu1.cost = 20.3;

    stu2.num = 2;
    memcpy(stu2.name, "wangqiang", sizeof(char) * 9);
    stu2.age = 25;
    //stu2.cost = 4550.32;
    stu2.cost = 30.5;

    FILE* fp = fopen("student", "wb+");
    if(fp == NULL)
    {
        printf("open file error: %s\n", strerror(errno));
        return -1;
    }

    fwrite(&stu1, sizeof(Student), 1, fp);
    fwrite(&stu2, sizeof(Student), 1, fp);
    fclose(fp);
    return 0;
}
