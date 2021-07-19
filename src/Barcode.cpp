#include "Barcode.h"
#include "SoftwareSerial.h"
#include <String.h>
#include <Arduino.h>

Barcode::Barcode(int rx, int tx) : _Rx(rx), _Tx(tx)
{
    SoftwareSerial BarcodeScanner(_Rx, _Tx); // rx=12, tx=14
}
int Barcode::detect()
{
}