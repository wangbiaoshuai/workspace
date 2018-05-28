#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

bool is_even(int& num)
{
    if(num % 2 == 0) 
        return true;
    else
        return false;
}

int main()
{
    vector<int> myvec;
    for(int i = 0; i < 10; i++)
        myvec.push_back(i);

    int evens = count_if(myvec.begin(), myvec.end(), is_even);

    cout<<"evens: "<<evens<<endl;
    return 0;
}
