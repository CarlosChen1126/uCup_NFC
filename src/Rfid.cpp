#include "Rfid.h"
#include <Arduino.h>
#include <MFRC522.h>

Rfid::Rfid(int Ss, int Rst) : _SSPin(Ss), _RSTPin(Rst)
{
    PCD_Init();
    PCD_DumpVersionToSerial();
}

void Rfid::arr2str(byte *arr, const unsigned int len, char *str)
{
    for (unsigned int i = 0; i < len; i++)
    {
        byte nib1 = (arr[i] >> 4) & 0x0F;
        byte nib2 = (arr[i] >> 0) & 0x0F;
        str[i * 2 + 0] = nib1 < 0xA ? '0' + nib1 : 'A' + nib1 - 0xA;
        str[i * 2 + 1] = nib2 < 0xA ? '0' + nib2 : 'A' + nib2 - 0xA;
    }
    str[len * 2] = '\0';
}

int Rfid::detect(String &Uid)
{
    int res = 0;
    byte uidd[4];
    char uid_char[8];
    if (!PICC_IsNewCardPresent() || !PICC_ReadCardSerial())
    {
        return 0;
    }
    PICC_Type piccType = PICC_GetType(uid.sak);
    if (piccType != PICC_TYPE_MIFARE_MINI &&
        piccType != PICC_TYPE_MIFARE_1K &&
        piccType != PICC_TYPE_MIFARE_4K)
    {
        Serial.println(F("Your tag is not of type MIFARE Classic."));
        return 0;
    }
    if (uid.uidByte[0] != Uid[0] ||
        uid.uidByte[1] != Uid[1] ||
        uid.uidByte[2] != Uid[2] ||
        uid.uidByte[3] != Uid[3])
    {
        res = 1;
        for (byte i = 0; i < 4; i++)
        {
            uidd[i] = uid.uidByte[i];
        }
        Serial.print("Uid: ");
        arr2str(uidd, 4, uid_char);
        String uid_tmp(uid_char);
        Uid = uid_tmp;
        //test uid
        Serial.print(Uid);
    }
    else
    {
        //same card
        res = 2;
    }
    PICC_HaltA();
    return res;
}
