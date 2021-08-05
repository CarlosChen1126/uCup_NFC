#include "Barcode.h"
#include "SoftwareSerial.h"
#include <Arduino.h>
//#include "String.h"

Barcode::Barcode(int rx, int tx) : _Rx(rx), _Tx(tx) {}
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
String Barcode::test()
{
    int index = 0;
    char id_barcode[36];
    if (available())
    {
        while (index < 36)
        {
            id_barcode[index] = (char)(read());
            index++;
        }
        id_barcode[index] = '\0';
        String stdid_tmp(id_barcode);
        return stdid_tmp;
    }
    return "qq";
}