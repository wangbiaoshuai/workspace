#include <stdio.h>
#include <wchar.h>
#include <iostream>
#include <string>
using namespace std;

int main()
{
    cout<<wcout.getloc().name()<<endl;
    //setlocale(LC_ALL,"");
    char mm[] = "C++\u2122";
    cout<<mm<<endl;
    printf("%s\n", mm);

    string ss("\u2122");
    printf("ss:%s\n", ss.c_str());

    char x[] = "\u626b\u63cf\u670d\u52a1";
    printf("%s\n", x);

    /*mm = 'h';
    printf("%c, %C\n", mm, mm);*/

    return 0;
}
