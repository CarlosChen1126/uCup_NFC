#pragma once
#include "Arduino.h"
//#include "String.h"
<<<<<<< HEAD
#include <SoftwareSerial.h>
=======
#include "SoftwareSerial.h"
>>>>>>> 49fa9d10ab0ca581e081b899ede5216ef315ef07
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