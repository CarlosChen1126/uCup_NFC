#pragma once
#include <Arduino.h>
class Barcode
{
public:
    Barcode(int, int);
    int detect();

private:
    int _Rx, _Tx;
    char stdID_b[9];
    char qrcode_b[36];
};