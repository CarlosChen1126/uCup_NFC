#include "Rfid.h"
#include <Arduino.h>
#include <MFRC522.h>

Rfid::Rfid(int Ss, int Rst) : _SSPin(Ss), _RSTPin(Rst)
{
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

// int Rfid::detect(String &uid)
// {
//     int res = 0;
//     byte uidd[4];
//     char uid_char[8];
//     if (!Rfid::Rfid.PICC_IsNewCardPresent() || Rfid::!Rfid.PICC_ReadCardSerial())
//     {
//         return 0;
//     }
//     MFRC522::PICC_Type piccType = Rfid::Rfid.PICC_GetType(Rfid.uid.sak);
//     if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
//         piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
//         piccType != MFRC522::PICC_TYPE_MIFARE_4K)
//     {
//         Serial.println(F("Your tag is not of type MIFARE Classic."));
//         return 0;
//     }
//     if (Rfid::Rfid.uid.uidByte[0] != uid[0] ||
//         Rfid::Rfid.uid.uidByte[1] != uid[1] ||
//         Rfid::Rfid.uid.uidByte[2] != uid[2] ||
//         Rfid::Rfid.uid.uidByte[3] != uid[3])
//     {
//         res = 1;
//         for (byte i = 0; i < 4; i++)
//         {
//             uidd[i] = Rfid::Rfid.uid.uidByte[i];
//         }
//         Serial.print("uid: ");
//         array_to_string(uidd, 4, uid_char);
//         String uid_tmp(uid_char);
//         uid = uid_tmp;
//         //test uid
//         Serial.print(uid);
//     }
//     else
//     {
//         //same card
//         res = 2;
//     }
//     Rfid::Rfid.PICC_HaltA();
//     return res;
// }
