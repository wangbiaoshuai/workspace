#include "fast_scan.h"

int main()
{
    FastScanItf* fast_scan = new Adapter();

    fast_scan->Request();
    delete fast_scan;
    return 0;
}
