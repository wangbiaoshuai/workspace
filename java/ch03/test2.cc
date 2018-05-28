#include <stdio.h>
#include <bitset>
#include <iostream>
using namespace std;

int main()
{
    int x = 4;
    int m = x << 3;
    printf("4<<3: %d\n", m);
    printf("-4<<3: %d\n", -4<<3);
    printf("16383<<1: %d\n", 16384<<1);

    short num = 16384;
    bitset<16> bit_num(num);
    cout<<bit_num<<endl;

    short num_1 = num << 3;
    cout<<num_1<<endl;

    unsigned char num_2 = 255;
    unsigned char num_3 = num_2 + 1;
    printf("%d\n", num_3);
    return 0;
}
