#include "Barcode.h"
#include "SoftwareSerial.h"
#include <String.h>
#include <Arduino.h>

Barcode::Barcode(int rx, int tx) : _Rx(rx), _Tx(tx)
{
    begin(115200);
}
int Barcode::detect(int len, String &ID)
{
    int index = 0;
    char id_barcode[len];
    if (available())
    {
        while (index < len)
        {
            id_barcode[index] = (char)(read());
            index++;
        }
        id_barcode[index] = '\0';
        String stdid_tmp(id_barcode);
        ID = stdid_tmp;
        return 1;
    }
    return 0;
}