#pragma once
#include <String.h>
#define RENT 0
#define RETURN 1
#define BIND_TIME 5000 //ms
class Config
{
public:
    // const char *ssid;     //Wifi name
    // const char *username; // Wifi User name
    // const char *password; // Wifi User password
    // String servername;    //Server's URL
    // int error_code;       // error code return from server
    // String stdID;         //User's Student ID
    // String uid;           // User's NFC ID
    // String qrcode;        // User's uCup line qrcode
    bool success;
    int button_cnt;
    Config();

private:
};
