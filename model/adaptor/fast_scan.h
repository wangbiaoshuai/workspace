#ifndef _FAST_SCAN_H
#define _FAST_SCAN_H

class FastScan
{
public:
    FastScan();
    ~FastScan();

    void Scaner();
};

class FastScanItf
{
public:
    FastScanItf();
    virtual ~FastScanItf();

    virtual void Request() = 0;
};

class Adapter : public FastScanItf
{
public:
    Adapter();
    ~Adapter();
    void Request();

private:
    FastScan fast_scan_;
};

#endif // _FAST_SCAN_H
