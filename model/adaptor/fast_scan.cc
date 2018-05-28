#include "fast_scan.h"
#include <iostream>
using namespace std;

FastScan::FastScan()
{
}

FastScan::~FastScan()
{
}

void FastScan::Scaner()
{
    cout<<"FastScan::Scaner"<<endl;
}

FastScanItf::FastScanItf()
{
}

FastScanItf::~FastScanItf()
{
}

Adapter::Adapter():
fast_scan_()
{
}

Adapter::~Adapter()
{
}

void Adapter::Request()
{
    fast_scan_.Scaner();
}
