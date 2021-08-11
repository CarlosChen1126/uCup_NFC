#include "WiFi.h"
//#include <String.h>
#include <SPI.h>
#include <Wire.h>
#include "esp_wpa2.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <Arduino.h>
#include "./src/Rfid.h"
#include "./src/Oled.h"
#include "./src/Button.h"
#include "./src/Config.h"
#include <SoftwareSerial.h>
//#include "./src/Barcode.h"
#include "./src/Buzzer.h"
#include "./src/LED.h"
#include "./src/MyServer.h"
//set WiFi name and password
//char *ssid = "carlos";
//char *passphrase = "carlosyoyo";

char *ssid = "CarlosChen";
char *passphrase = "iloveangel";
// char *ssid = "MakerSpace_2.4G";
// char *passphrase = "ntueesaad";

// #define EAP_IDENTITY "b08901048"  //if connecting from another corporation, use identity@organisation.domain in Eduroam
// #define EAP_PASSWORD "Carlos1126" //your Eduroam password
// const char *ssid = "ntu_peap";
#define RENT 0
#define RETURN 1
#define BIND_TIME 8000 //ms

//hi
String token;
char std_id_barcode[36];
const int BUZZ_PIN = 2;   //Buzzer pin
const int LED_RED = 27;   //Red LED pin
const int LED_GREEN = 13; //Blue LED pin
const int RST_PIN = 36;   // Reset pin
const int SS_PIN = 5;     // Slave select pin
//const String serverName = "https://ucup-dev.herokuapp.com/api";
//SoftwareSerial BarcodeScanner(12, 14); //rx,tx //barcode
//U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE); //OLED
Oled oled(U8G2_R0, SCL, SDA, U8X8_PIN_NONE);
Button button(17);
Config config;
SoftwareSerial barcode(14, 12);
Buzzer buzzer(2);
LED LED_R(27);
LED LED_G(13);
MyServer server;
Rfid rc522(19, 20);

bool testWifi(void)
{
  int c = 0;
  Serial.println("Waiting for Wifi to connect");
  while (c < 20)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      return true;
    }
    delay(500);
    Serial.print(WiFi.status());
    c++;
  }
  return false;
}
int detect_barcode(int len, String &ID)
{
  int index = 0;
  if (barcode.available())
  {
    int start_time = millis();
    while (index < len && millis() - start_time < 100)
    {
      if (barcode.available())
      {
        std_id_barcode[index] = (char)(barcode.read());
        index++;
      }
    }
    std_id_barcode[index] = '\0';
    String stdid_tmp(std_id_barcode);
    ID = stdid_tmp;
    return 1;
  }
  return 0;
}
void finish()
{
  config.last_qrcode = config.qrcode;
  config.last_uid = config.uid;
  config.qrcode = "";
  config.stdID = "";
  config.uid = "";
  config.success = true;
  config.rfid_work = 0;
  config.barcode_work = 0;
  config.error_code = 0;
  config.button_cnt = 0;
}
void setup()
{
  Serial.begin(115200); // Initialize serial communications with the PCSerial.begin(9600);
  //begin WiFi
  WiFi.begin(ssid, passphrase);
  barcode.begin(9600);
  Serial.print("Connecting to network: ");
  Serial.println(ssid);
  // WiFi.disconnect(true);                                                             //disconnect form wifi to set new wifi connection
  // WiFi.mode(WIFI_STA);                                                               //init wifi mode
  // esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY)); //provide identity
  // esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY)); //provide username --> identity and username is same
  // esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD)); //provide password
  // esp_wpa2_config_t esp_config = WPA2_CONFIG_INIT_DEFAULT();                         //set config settings to default
  // esp_wifi_sta_wpa2_ent_enable(&esp_config);                                         //set config settings to enable function
  // WiFi.begin(ssid);                                                                  //connect to wifi
  // int counter = 0;
  // while (WiFi.status() != WL_CONNECTED)
  // {
  //   delay(500);
  //   Serial.print(".");
  //   counter++;
  //   if (counter >= 60)
  //   { //after 30 seconds timeout - reset board
  //     ESP.restart();
  //   }
  // }
  // Serial.println("");
  // Serial.println("WiFi connected");
  // Serial.println("IP address set: ");
  // Serial.println(WiFi.localIP()); //print LAN IP
  oled.twolines_en("Welcome", "uCup");

  if (testWifi())
  {
    //show WiFi IP address
    Serial.println("WiFi connected OK");
    Serial.print("Local IP: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    //log WiFi error
    Serial.println("WiFi connected NG");
  }
  SPI.begin();                     // Init SPI bus
  rc522.PCD_Init();                // Init MFRC522
  rc522.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
  delay(2000);
  //u8g2.clearBuffer();
  oled.clearBuffer();
  LED_G.blink(1000);
  LED_R.blink(1000);
  config.token = server.GetToken();
  Serial.println("config.token:");
  Serial.println(config.token);
  if (config.token == 0)
  {
    oled.printc(15, 15, "請重新開機");
  }
  oled.printe(15, 15, "Touch to Start");
}

void loop()
{
  // manage the operation mode
  button.click(config.button_cnt, config.success);
  switch (config.button_cnt % 2)
  {
  case RENT:
  {
    //rent
    oled.twolines("租借模式", "請感應學生證");
    config.rfid_work = rc522.detect(config.uid);
    config.barcode_work = detect_barcode(36, config.qrcode);
    //show rent message
    if (config.rfid_work == 1)
    {

      oled.printc(35, 40, "載入中");

      Serial.println("uid:");
      Serial.println(rc522.detect(config.uid));
      int http_code = server.CupRecord(config.token, config.uid, "NFC", "uCup", "/do_rent", config.error_code);
      if (http_code == 200)
      {
        oled.twolines("租借成功", "謝謝惠顧");
        Serial.println("rent 200");
        LED_G.blink(2000);
        finish();
        delay(1000);
      }
      else
      {
        if (config.error_code == 1)
        {

          //not registered
          //「 租借失敗 」
          //「 請先註冊uCup會員 」
          oled.twolines("租借失敗", "請先註冊會員");
          Serial.println("rent rfid 1");
          delay(2500);
          Serial.println("direct to bind mode");

          oled.twolines("綁定模式", "請掃描學生證條碼");
          int bind_start_time = millis();
          int bind_http_code = 0;
          while (millis() - bind_start_time < BIND_TIME)
          {
            int bind_work = detect_barcode(9, config.stdID);
            if (bind_work == 1)
            {
              bind_http_code = server.CupBind(config.token, config.uid, config.stdID);
              break;
            }
          }
          if (bind_http_code == 200)
          {
            //bind success
            // u8g2.clearBuffer();
            // u8g2_print_ch(0, 15, "綁定成功");
            // u8g2_print_ch(0, 40, "請重新操作");
            // u8g2.sendBuffer();
            oled.twolines("綁定成功", "請重新操作");
            Serial.println("bind success");
            finish();
            LED_G.blink(1000);
          }
          else
          {
            // u8g2.clearBuffer();
            // u8g2_print_ch(0, 15, "綁定失敗");
            // u8g2_print_ch(0, 40, "請重新操作");
            // u8g2.sendBuffer();
            oled.twolines("綁定失敗", "請重新操作");
            Serial.println("bind fail");
            finish();
            LED_R.blink(1000);
          }
          delay(2000);
        }
        else if (config.error_code == 2 || config.error_code == 21)
        {
          Serial.println("rent rfid 2");
          oled.twolines("租借失敗", "請先歸還杯子");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "租借失敗");
          // u8g2_print_ch(0, 40, "請先歸還杯子");
          // u8g2.sendBuffer();
          //last borrowed cup not return
          //「 租借失敗 」
          //「 請先歸還杯子 」
          finish();
          LED_R.blink(2000);
          delay(1000);
        }
        else if (config.error_code == 3)
        {
          Serial.println("rent rfid 3");
          oled.twolines("租借失敗", "上次租借未滿30分鐘");

          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "租借失敗");
          // u8g2_print_ch(0, 40, "上次租借未滿30分鐘");
          // u8g2.sendBuffer();
          //last borrowed less than 30mins
          //「 租借失敗 」
          //「 上次租借未滿30分鐘」
          finish();
          LED_R.blink(2000);
          delay(1000);
        }
        else if (config.error_code == 4)
        {
          Serial.println("rent rfid 4");
          oled.twolines("租借失敗", "商店杯子不足");
          finish();
          LED_R.blink(2000);
          delay(1000);
        }
        else if (config.error_code == 5)
        {
          Serial.print("rent 5");
          oled.twolines("租借失敗", "請先註冊帳號");
          finish();
          LED_R.blink(2000);
          delay(1000);
        }
      }
    }
    else if (config.barcode_work == 1)
    {

      Serial.print("qrcode: ");
      Serial.println(config.qrcode);
      if (config.last_qrcode == config.qrcode)
      {
        Serial.println("Same qrcode again");
        break;
      }
      int http_code = server.CupRecord(config.token, config.qrcode, "Normal", "uCup", "/do_rent", config.error_code);
      if (http_code == 200)
      {
        Serial.println("rent qrcode success");
        oled.twolines("租借成功", "謝謝惠顧");
        finish();
        LED_G.blink(2000);
        delay(1000);
        //success
        //TODO: print success text
      }
      else
      {
        if (config.error_code == 1)
        {
          Serial.println("rent qrcode 1");
          oled.twolines("租借失敗", "請先註冊會員");
          finish();
          LED_R.blink(2000);
          delay(1000);
          //not registered
          //「 租借失敗 」
          //「 請先註冊uCup會員 」
        }
        else if (config.error_code == 2 || config.error_code == 21)
        {
          Serial.println("rent qrcode 2");
          oled.twolines("租借失敗", "請先歸還杯子");
          finish();
          LED_R.blink(2000);
          delay(1000);
          //last borrowed cup not return
          //「 租借失敗 」
          //「 請先歸還杯子 」
        }
        else if (config.error_code == 3)
        {
          Serial.println("rent qrcode 3");
          oled.twolines("租借失敗", "上次租借未滿30分鐘");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "租借失敗");
          // u8g2_print_ch(0, 40, "上次租借未滿30分鐘");
          // u8g2.sendBuffer();
          finish();
          LED_R.blink(2000);
          delay(1000);
          //last borrowed less than 30mins
          //「 租借失敗 」
          //「 上次租借未滿30分鐘」
        }
        else if (config.error_code == 4)
        {
          Serial.println("rent qrcode 4");
          oled.twolines("租借失敗", "商店杯子不足");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "租借失敗");
          // u8g2_print_ch(0, 40, "商店杯子不足");
          // u8g2.sendBuffer();
          finish();
          LED_R.blink(2000);
          delay(1000);
          //cups in the store < 3
          //「 租借失敗 」
          //「 商店杯子不足 」
        }
        else if (config.error_code == 5)
        {
          Serial.println("rent qrcode 5");
          oled.twolines("租借失敗", "請先綁定帳號");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "租借失敗");
          // u8g2_print_ch(0, 40, "請先綁定帳號");
          // u8g2.sendBuffer();
          // not verified
          //「 租借失敗 」
          //「請先綁定帳號」
          finish();
          LED_R.blink(2000);
          delay(1000);
        }
      }
    }
    break;
  }
  case RETURN:
  {
    //return
    config.rfid_work = rc522.detect(config.uid);
    config.barcode_work = detect_barcode(36, config.qrcode);
    oled.twolines("歸還模式", "請感應學生證");
    //show return message
    if (config.rfid_work == 1)
    {
      Serial.print("uid: ");
      Serial.println(config.uid);
      int http_code = server.CupRecord(config.token, config.uid, "NFC", "uCup", "/do_return", config.error_code);
      if (http_code == 200)
      {
        // u8g2.clearBuffer();
        // u8g2_print_ch(0, 15, "歸還成功");
        // u8g2_print_ch(0, 40, "謝謝惠顧");
        // u8g2.sendBuffer();
        oled.twolines("歸還成功", "謝謝惠顧");
        Serial.println("rent 200");
        //success
        //TODO: print success text
        //「 歸還成功 」
        //「 謝謝惠顧 」
        LED_G.blink(2000);
        finish();
        delay(1000);
      }
      else
      {
        if (config.error_code == 1)
        {
          Serial.println("return rfid 1");
          oled.twolines("歸還失敗", "請先註冊會員");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "歸還失敗");
          // u8g2_print_ch(0, 40, "請先註冊會員");
          // u8g2.sendBuffer();
          Serial.println("rent rfid 1");
          LED_R.blink(2000);
          Serial.println("direct to bind mode");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "綁定模式");
          // u8g2_print_ch(0, 40, "請掃描學生證條碼");
          // u8g2.sendBuffer();
          oled.twolines("綁定模式", "請掃描學生證條碼");
          int bind_start_time = millis();
          int bind_http_code = 0;
          while (millis() - bind_start_time < BIND_TIME)
          {
            int bind_work = 0;
            if (bind_work == 1)
            {
              bind_http_code = server.CupBind(config.token, config.uid, config.stdID);
              break;
            }
          }
          if (bind_http_code == 200)
          {
            oled.twolines("綁定成功", "請重新操作");
            Serial.println("bind success");
            finish();
            LED_G.blink(2000);
          }
          else
          {
            oled.twolines("綁定失敗", "請重新操作");
            Serial.println("bind fail");
            finish();
            LED_R.blink(2000);
          }

          delay(1000);
          //not registered
        }
        else if (config.error_code == 2 || config.error_code == 21)
        {
          Serial.println("return rfid 2");
          oled.twolines("歸還失敗", "請先歸還杯子");
          finish();
          LED_R.blink(2000);
          delay(1000);
          //last borrowed cup not return
        }
        else if (config.error_code == 3)
        {
          Serial.println("return rfid 3");
          oled.twolines("歸還失敗", "上次歸還未滿30分鐘");
          finish();
          LED_R.blink(2000);
          delay(1000);

          //last borrowed less than 30mins
        }
        else if (config.error_code == 4)
        {
          Serial.println("return rfid 4");
          oled.twolines("歸還失敗", "商店杯子不足");
          finish();
          LED_R.blink(2000);
          delay(1000);
          //cups in the store < 3
          //「 歸還失敗 」
          //「 商店杯子不足 」
        }
        else if (config.error_code == 5)
        {
          Serial.println("return rfid 5");
          oled.twolines("歸還失敗", "請先註冊帳號");
          finish();
          LED_R.blink(2000);
          delay(1000);
          // not verified
          //「 歸還失敗 」
          //「」
        }
      }
    }
    else if (config.barcode_work == 1)
    {
      Serial.print("qrcode: ");
      Serial.println(config.qrcode);
      int http_code = server.CupRecord(config.token, config.qrcode, "Normal", "uCup", "/do_return", config.error_code);
      if (http_code == 200)
      {
        Serial.println("return qrcode success");
        oled.twolines("歸還成功", "謝謝惠顧");
        finish();
        LED_G.blink(2000);
        delay(1000);
        //success
        //TODO: print success text
      }
      else
      {
        if (config.error_code == 1)
        {
          Serial.println("return qrcode 1");
          oled.twolines("歸還失敗", "請先註冊會員");
          finish();
          LED_R.blink(2000);
          delay(1000);
          //not registered
        }
        else if (config.error_code == 2 || config.error_code == 21)
        {
          Serial.println("return qrcode 2");
          oled.twolines("歸還失敗", "請先歸還杯子");
          finish();
          LED_R.blink(2000);
          delay(1000);
          //last borrowed cup not return
        }
        else if (config.error_code == 3)
        {
          Serial.println("return qrcode 3");
          oled.twolines("歸還失敗", "上次歸還未滿30分鐘");
          finish();
          LED_R.blink(2000);
          delay(1000);
          //last borrowed less than 30mins
          //「 歸還失敗 」
          //「 上次租借未滿30分鐘」
        }
        else if (config.error_code == 4)
        {
          Serial.println("return qrcode 4");
          oled.twolines("歸還失敗", "商店杯子不足");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "歸還失敗");
          // u8g2_print_ch(0, 40, "商店杯子不足");
          // u8g2.sendBuffer();
          finish();
          LED_R.blink(2000);
          delay(1000);
          //cups in the store < 3
        }
        else if (config.error_code == 5)
        {
          Serial.println("return qrcode 5");
          oled.twolines("歸還失敗", "請先註冊帳號");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "歸還失敗");
          // u8g2_print_ch(0, 40, "請先綁定帳號");
          // u8g2.sendBuffer();
          // not verified
          finish();
          LED_R.blink(2000);
          delay(1000);
        }
      }
    }

    break;
  }
  }
}
