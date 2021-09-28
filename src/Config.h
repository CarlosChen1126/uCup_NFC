#pragma once
#define RENT 0
#define RETURN 1
#define BIND_TIME 5000 //ms
#include <Arduino.h>
//#include <String.h>
//const String servername = "https://ucup-dev.herokuapp.com/api"; //test
const String servername = "https://better-u-cup.herokuapp.com/api"; //production
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
    String last_uid = "";
    String last_qrcode = "";
    //直營店
    // char *ssid = "MakerSpace_2.4G";
    // char *passphrase = "ntueesaad";
    // String Account = "0900000000";
    // String Password = "choosebetterbebetter";

    //原典
    //String Account = "83695755";
    //String Password = "83695755";

    //大一女比斯多
    //String Account = "0935350223";
    //String Password = "121535066";

    //男一早餐店
    // const char *ssid = "12345678";
    // const char *passphrase = "12345678";
    // String Account = "0955579263";
    // String Password = "121544701";

    //玉珍圜台大校園門市
    //const char *ssid = "yzy888";
    //const char *passphrase = "66kk0616";
    //String Account = "0922441389";
    //String Password = "121537853";

    //水雲間
    // const char *ssid = "sally";
    // const char *passphrase = "23661686";
    // String Account = "0977400989";
    // String Password = "0977400989";

    // const char *ssid = "MakerSpace_2.4G";
    // const char *passphrase = "ntueesaad";

    //teashop
    // const char *ssid = "Magic";
    // const char *passphrase = "27380886";
    // String Account = "teashop";
    // String Password = "27380886";

    //小鹿亂撞
    // const char *ssid = "ichefpos_C";
    // const char *passphrase = "52609717";
    // String Account = "dokidoki";
    // String Password = "dokidoki";

    //椰林
    // const char *ssid = "Dcoffee";
    // const char *passphrase = "0223684222";
    // String Account = "yelincabins";
    // String Password = "yelincabins";

    //黛黛
    // const char *ssid = "Daily Dae";
    // const char *passphrase = "23685950";
    // String Account = "daliday";
    // String Password = "23685950";

    //龜記
    // const char *ssid = "guiji2.4";
    // const char *passphrase = "23660667";
    // String Account = "guijigroup";
    // String Password = "isgood666";

    //59
    // const char *ssid = "ZTE_B19BEC";
    // const char *passphrase = "94694742";
    // String Account = "fivenine59";
    // String Password = "isawesome666";

    //墾丁
    // const char *ssid = "ZTE_B19A2F";
    // const char *passphrase = "25502663";
    // String Account = "kentingmilk";
    // String Password = "isbetter666";

    //女9果汁店
    // String Account = "";
    // String Password = "";
};
