#include <string>
#include <algorithm>
#include <iostream>

using namespace std;

int main()
{
    string str1 = "This is a test.";
    string str2(30, '\0');

    remove_copy(str1.begin(), str1.end(), str2.begin(), ' ');

    cout<<"str1:"<<str1<<endl;
    cout<<"str2:"<<str2<<endl;
    return 0;
}
