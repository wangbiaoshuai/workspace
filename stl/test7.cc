#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

int main()
{
    string str = "kongyue is a pig.";

    reverse(str.begin(), str.end());

    cout<<"str:"<<str<<endl;
    return 0;
}
