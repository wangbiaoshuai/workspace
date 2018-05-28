#include <stdio.h>
#include <string.h>
#include <fstream>
#include <sstream>
using namespace std;

string reverse(const string& str)
{
    string res;
    if(str.empty())
        return res;

    for(int i = str.length() - 1; i >= 0; i--)
    {
        res += str[i];
    }
    return res;
}

int reverse_copy(const string& file1, const string& file2)
{
    if(file1.empty() || file2.empty())
        return -1;

    ifstream in_file;
    in_file.open(file1.c_str(), ios::in);
    char c = in_file.get();

    stringstream ss;
    while (in_file.good()) {
        ss << c;
        c = in_file.get();
    }
    in_file.close();

    string tmp = reverse(ss.str());

    ofstream out_file;
    out_file.open(file2.c_str(), ios::out | ios::binary);
    out_file<<tmp;
    out_file.close();
    return 0;
}

int main(void)
{
    string file1 = "file1.txt";
    string file2 = "file2.txt";
    reverse_copy(file1, file2);
    return 0;
}
