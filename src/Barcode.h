#pragma once
#include "Arduino.h"
//#include "String.h"
#include <SoftwareSerial.h>
class Barcode : public SoftwareSerial
{
public:
    Barcode(int, int);
    int detect(int, String &);
    String test();

private:
    int _Rx, _Tx;
    char stdID_b[9];
    char qrcode_b[36];
};