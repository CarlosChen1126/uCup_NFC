#pragma once
#include <string.h>
#include <MFRC522.h>

class Rfid : public MFRC522
{
public:
    Rfid(int, int);
    void arr2str(byte *, const unsigned int, char *);
    int detect(String &);

private:
    int _SSPin, _RSTPin;
}

//WARNING!!!! mfrc522 is an obj of MFRC522 (NEED to be solved)