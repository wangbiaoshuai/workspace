#include <stdio.h>
#include <string.h>
#include <errno.h>

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

void print_stu(const Student* stu)
{
    if(stu == NULL)
    {
        return;
    }

    printf("num: %d\n", stu->num);
    printf("name: %s\n", stu->name);
    printf("age: %d\n", stu->age);
    printf("cost: %lf\n", stu->cost);
    printf("\n");
    return;
}

int main(void)
{
    Student stu1, stu2;

    FILE* fp = fopen("student", "rb");
    if(fp == NULL)
    {
        printf("open file error: %s\n", strerror(errno));
        return -1;
    }

    fread(&stu1, sizeof(Student), 1, fp);
    fread(&stu2, sizeof(Student), 1, fp);
    fclose(fp);

    print_stu(&stu1);
    print_stu(&stu2);
    return 0;
}
