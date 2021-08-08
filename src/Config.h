#pragma once
#define RENT 0
#define RETURN 1
#define BIND_TIME 5000 //ms
#include <Arduino.h>
//#include <String.h>
const String servername = "https://ucup-dev.herokuapp.com/api";
struct Config
{
    // const char *ssid;     //Wifi name
    // const char *username; // Wifi User name
    // const char *password; // Wifi User password
    //String servername = "https://ucup-dev.herokuapp.com/api"; //Server's URL
    int error_code = 0; // error code return from server
    String stdID = "";  //User's Student ID
    String uid = "";    // User's NFC ID
    String qrcode = ""; // User's uCup line qrcode
    String token = "";
    bool success = false;
    int button_cnt = -1;
    int rfid_work = 0;
    int barcode_work = 0;
};
