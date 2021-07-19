#pragma once
class Rfid
{
public:
    Rfid(int, int);
    void arr2str(byte *, const unsigned int, char *);
    int detect();

private:
    int _SSPin, _RSTPin;
}