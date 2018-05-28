#include <iostream>
#include <string>
#include "vector_map.h"
using namespace std;

int main()
{
    vector_map<int, string> vm;
    //vm.push_back(pair<int, string>(1, "wangbiaoshuai"));
    vm.push_back(1, "wangbs");
    return 0;
}
