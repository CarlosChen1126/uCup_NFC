#include <String.h>
#pragma once
class Rfid
{
public:
    Rfid(int, int);
    void arr2str(byte *, const unsigned int, char *);
    int detect(String &);

private:
    int _SSPin, _RSTPin;
}

//WARNING!!!! mfrc522 is an obj of MFRC522 (NEED to be solved)