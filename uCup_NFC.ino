#include "WiFi.h"
#include <String.h>
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
#include "./src/Barcode.h"
#include "./src/Buzzer.h"
#include "./src/LED.h"
#include "./src/MyServer.h"
//set WiFi name and password
// char *ssid = "carlos";
// char *passphrase = "carlosyoyo";

// char *ssid = "MakerSpace_2.4G";
// char *passphrase = "ntueesaad";

#define EAP_IDENTITY "b08901048"  //if connecting from another corporation, use identity@organisation.domain in Eduroam
#define EAP_PASSWORD "Carlos1126" //your Eduroam password
const char *ssid = "ntu_peap";
#define RENT 0
#define RETURN 1
#define BIND_TIME 5000 //ms

//hi
String token;
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
Barcode barcode(12, 14);
Buzzer buzzer(2);
LED LED_R(27);
LED LED_G(13);
MyServer server;
// byte uidd[4];
// char uid_char[9];
// String uid;
// char std_id_barcode[9];
// char qrcode_barcode[36];
// String stdid;
// String qrcode;
// bool is_card_valid = false;
// int error_code = -1;
// bool success = false;
//MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance
Rfid rc522(SS_PIN, RST_PIN);
// bool testWifi(void)
// {
//   int c = 0;
//   Serial.println("Waiting for Wifi to connect");
//   while (c < 20)
//   {
//     if (WiFi.status() == WL_CONNECTED)
//     {
//       return true;
//     }
//     delay(500);
//     Serial.print(WiFi.status());
//     c++;
//   }
//   return false;
// }
// void array_to_string(byte array[], unsigned int len, char buffer[])
// {
//   for (unsigned int i = 0; i < len; i++)
//   {
//     byte nib1 = (array[i] >> 4) & 0x0F;
//     byte nib2 = (array[i] >> 0) & 0x0F;
//     buffer[i * 2 + 0] = nib1 < 0xA ? '0' + nib1 : 'A' + nib1 - 0xA;
//     buffer[i * 2 + 1] = nib2 < 0xA ? '0' + nib2 : 'A' + nib2 - 0xA;
//   }
//   buffer[len * 2] = '\0';
// }

// String gettoken()
// {
//   if (WiFi.status() == WL_CONNECTED)
//   {
//     HTTPClient http;

//     // Your Domain name with URL path or IP address with path
//     http.begin(config.servername + "/stores/login");

//     // Specify content-type header
//     http.addHeader("Content-Type", "application/x-www-form-urlencoded");
//     // Data to send with HTTP POST
//     String httpRequestData = "phone=0900000000&password=choosebetterbebetter";
//     // Send HTTP POST request
//     int httpResponseCode = http.POST(httpRequestData);
//     StaticJsonDocument<900> doc;
//     DeserializationError error = deserializeJson(doc, http.getString());

//     // log err
//     if (error)
//     {
//       Serial.print(F("deserializeJson() failed: "));
//       Serial.println(error.f_str());
//       return "0";
//     }

//     //parse the data to get token
//     String token = doc["token"];
//     token = "Bearer " + token;
//     Serial.println("token:");
//     Serial.println(token);
//     return token;
//   }
//   else
//   {
//     return "WIFI disconnected.";
//   }
// }
int cup_record(String token, String stdID, String provider, String type, String operation)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;

    http.begin(servername + "/record" + operation);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("Authorization", token);

    String httpRequestData = "user_id=" + stdID + "&provider=" + provider + "&cup_type=" + type;
    //log info of http req
    Serial.println(servername + "/record" + operation);
    Serial.print("token: ");
    Serial.println(token);
    Serial.print("req data: ");
    Serial.println(httpRequestData);
    Serial.print("operation: ");
    Serial.println(operation);
    StaticJsonDocument<1500> doc;
    int httpResponseCode = http.POST(httpRequestData);
    String res = http.getString();
    DeserializationError error = deserializeJson(doc, res);
    Serial.print("cup_httpcode: ");
    Serial.println(httpResponseCode);
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return 87;
    }

    config.error_code = doc["error_code"];
    Serial.print("error_code: ");
    Serial.println(config.error_code);
    return httpResponseCode;
  }
  else
  {
    return 87;
  }
}
int cup_bind(String token, String nfc_id, String ntu_id)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;

    http.begin(servername + "/users/bind_ntu_nfc");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("Authorization", token);

    String httpRequestData = "nfc_id=" + nfc_id + "&ntu_id=" + ntu_id;
    //log info of http req
    Serial.println(servername + "/users/bind_ntu_nfc");
    Serial.print("token: ");
    Serial.println(token);
    Serial.print("req data: ");
    Serial.println(httpRequestData);
    Serial.println("binding");
    StaticJsonDocument<500> doc;
    int httpResponseCode = http.POST(httpRequestData);
    String res = http.getString();
    DeserializationError error = deserializeJson(doc, res);

    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return 87;
    }

    int res_code = doc["error_record"];
    Serial.print("cup_httpcode: ");
    Serial.println(httpResponseCode);
    Serial.print("error_code: ");
    Serial.println(res_code);
    return httpResponseCode;
  }
  else
  {
    return 87;
  }
}
void setup()
{
  Serial.begin(9600); // Initialize serial communications with the PCSerial.begin(9600);
  //begin WiFi
  //WiFi.begin(ssid, passphrase);
  //begin barcode
  //BarcodeScanner.begin(9600);
  //pin for button
  //pinMode(17, INPUT_PULLUP);
  Serial.print("Connecting to network: ");
  Serial.println(ssid);
  WiFi.disconnect(true);                                                             //disconnect form wifi to set new wifi connection
  WiFi.mode(WIFI_STA);                                                               //init wifi mode
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY)); //provide identity
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY)); //provide username --> identity and username is same
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD)); //provide password
  esp_wpa2_config_t esp_config = WPA2_CONFIG_INIT_DEFAULT();                         //set config settings to default
  esp_wifi_sta_wpa2_ent_enable(&esp_config);                                         //set config settings to enable function
  WiFi.begin(ssid);                                                                  //connect to wifi
  int counter = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    counter++;
    if (counter >= 60)
    { //after 30 seconds timeout - reset board
      ESP.restart();
    }
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address set: ");
  Serial.println(WiFi.localIP()); //print LAN IP
  //init Buzzer
  //pinMode(BUZZ_PIN, OUTPUT);
  //init LED
  // pinMode(LED_GREEN, OUTPUT);
  // pinMode(LED_RED, OUTPUT);
  //begin OLED
  // u8g2.begin();
  // u8g2.enableUTF8Print();
  // u8g2.clearBuffer();
  // u8g2_print_en(15, 15, "Welcome");
  // u8g2_print_en(15, 40, "uCup");
  oled.twolines_en("Welcome", "uCup");

  //  if (testWifi())
  //  {
  //    //show WiFi IP address
  //    Serial.println("WiFi connected OK");
  //    Serial.print("Local IP: ");
  //    Serial.println(WiFi.localIP());
  //  }
  //  else
  //  {
  //    //log WiFi error
  //    Serial.println("WiFi connected NG");
  //  }
  SPI.begin(); // Init SPI bus
  //mfrc522.PCD_Init();                // Init MFRC522
  //mfrc522.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
  delay(2000);
  //u8g2.clearBuffer();
  oled.clearBuffer();

  config.token = server.GetToken();
  Serial.println("config.token:");
  Serial.println(config.token);
}

void loop()
{
  // manage the operation mode
  // Serial.print("Token: ");
  // Serial.println(token);
  // int button = digitalRead(17);
  // if (button == 0)
  // {
  //   button_ctn += 1;
  //   success = false;
  // }
  button.click(config.button_cnt, config.success);

  switch (config.button_cnt % 2)
  {
  case RENT:
  {
    //rent
    int rfid_work = rc522.detect(config.uid);
    int barcode_work = barcode.detect(36, config.qrcode);
    int start_rent_time = 0;
    start_rent_time = millis();
    // u8g2.clearBuffer();
    // u8g2_print_ch(0, 15, "租借模式");
    // u8g2_print_ch(0, 60, "請感應學生證");
    // u8g2.sendBuffer();
    oled.twolines("租借模式", "請感應學生證");
    //show rent message
    //      if (button == 0){
    //        button_ctn += 1;
    //        success = false;
    //      }
    if (rfid_work == 1)
    {
      Serial.println("uid:");
      Serial.println(rc522.detect(config.uid));
      int http_code = server.CupRecord(config.token, config.uid, "NFC", "uCup", "/do_rent", config.error_code);
      if (http_code == 200)
      {
        // u8g2.clearBuffer();
        // u8g2_print_ch(0, 15, "租借成功");
        // u8g2_print_ch(0, 40, "謝謝惠顧");
        // u8g2.sendBuffer();
        oled.twolines("租借成功", "謝謝惠顧");
        Serial.println("rent 200");
        LED_G.blink(1500);
        //success
        //TODO: print success text
        //「 租借成功 」
        //「 謝謝惠顧 」
        config.qrcode = "";
        config.stdID = "";
        config.uid = "";
        config.success = true;
        delay(1500);
      }
      else
      {
        if (config.error_code == 1)
        {
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "租借失敗");
          // u8g2_print_ch(0, 40, "請先註冊uCup會員");
          // u8g2.sendBuffer();

          //not registered
          //「 租借失敗 」
          //「 請先註冊uCup會員 」
          oled.twolines("租借失敗", "請先註冊會員");
          Serial.println("rent rfid 1");
          delay(2000);
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
            int bind_work = barcode.detect(9, config.stdID);
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
            config.success = true;
          }
          else
          {
            // u8g2.clearBuffer();
            // u8g2_print_ch(0, 15, "綁定失敗");
            // u8g2_print_ch(0, 40, "請重新操作");
            // u8g2.sendBuffer();
            oled.twolines("綁定失敗", "請重新操作");
            Serial.println("bind fail");
            config.success = true;
          }
          delay(3000);
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
          delay(3000);
          config.success = true;
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
          delay(3000);
        }
        else if (config.error_code == 4)
        {
          Serial.println("rent rfid 4");
          oled.twolines("租借失敗", "商店杯子不足");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "租借失敗");
          // u8g2_print_ch(0, 40, "商店杯子不足");
          // u8g2.sendBuffer();
          //cups in the store < 3
          //「 租借失敗 」
          //「 商店杯子不足 」
          delay(3000);
        }
        else if (config.error_code == 5)
        {
          Serial.print("rent 5");
          oled.twolines("租借失敗", "請先註冊帳號");

          // not verified
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "租借失敗");
          // u8g2_print_ch(0, 40, "請先註冊帳號");
          // u8g2.sendBuffer();
          //「 租借失敗 」
          //「請先綁定帳號」
          delay(3000);
        }
      }
    }
    else if (barcode_work == 1)
    {
      Serial.print("qrcode: ");
      Serial.println(config.qrcode);
      int http_code = server.CupRecord(config.token, config.qrcode, "Normal", "uCup", "/do_rent", config.error_code);
      if (http_code == 200)
      {
        Serial.println("rent qrcode success");
        oled.twolines("租借成功", "謝謝惠顧");
        // u8g2.clearBuffer();
        // u8g2_print_ch(0, 15, "租借成功");
        // u8g2_print_ch(0, 40, "謝謝惠顧");
        // u8g2.sendBuffer();
        config.qrcode = "";
        config.stdID = "";
        config.uid = "";
        config.success = true;
        delay(3000);
        //success
        //TODO: print success text
      }
      else
      {
        if (config.error_code == 1)
        {
          Serial.println("rent qrcode 1");
          oled.twolines("租借失敗", "請先註冊會員");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "租借失敗");
          // u8g2_print_ch(0, 40, "請先註冊會員");
          // u8g2.sendBuffer();
          delay(3000);
          //not registered
          //「 租借失敗 」
          //「 請先註冊uCup會員 」
        }
        else if (config.error_code == 2 || config.error_code == 21)
        {
          Serial.println("rent qrcode 2");
          oled.twolines("租借失敗", "請先歸還杯子");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "租借失敗");
          // u8g2_print_ch(0, 40, "請先歸還杯子");
          // u8g2.sendBuffer();
          delay(3000);
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
          delay(3000);
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
          delay(3000);
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
          delay(3000);
        }
      }
    }
    break;
  }
  case RETURN:
  {
    //return
    int rfid_work = rc522.detect(config.uid);
    int barcode_work = barcode.detect(36, config.qrcode);
    int start_return_time = 0;
    start_return_time = millis();
    oled.twolines("歸還模式", "請感應學生證");
    // u8g2.clearBuffer();
    // u8g2_print_ch(0, 15, "歸還模式");
    // u8g2_print_ch(0, 40, "請感應學生證");
    // u8g2.sendBuffer();

    //      if (button == 0){
    //        button_ctn += 1;
    //        success = false;
    //      }
    //show return message
    if (rfid_work == 1)
    {
      Serial.print("uid: ");
      Serial.println(config.uid);
      int http_code = server.CupRecord(config.token, config.uid, "NFC", "uCup", "./do_return", config.error_code);
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

        config.success = true;
        config.qrcode = "";
        config.stdID = "";
        config.uid = "";
        delay(3000);
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
          delay(2000);
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
            int bind_work = barcode.detect(9, config.stdID);
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
            config.success = true;
          }
          else
          {
            // u8g2.clearBuffer();
            // u8g2_print_ch(0, 15, "綁定失敗");
            // u8g2_print_ch(0, 40, "請重新操作");
            // u8g2.sendBuffer();
            oled.twolines("綁定失敗", "請重新操作");
            Serial.println("bind fail");
            config.success = true;
          }

          delay(3000);
          //not registered
        }
        else if (config.error_code == 2 || config.error_code == 21)
        {
          Serial.println("return rfid 2");
          oled.twolines("歸還失敗", "請先歸還杯子");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "歸還失敗");
          // u8g2_print_ch(0, 40, "請先歸還杯子");
          // u8g2.sendBuffer();
          delay(3000);
          //last borrowed cup not return
        }
        else if (config.error_code == 3)
        {
          Serial.println("return rfid 3");
          oled.twolines("歸還失敗", "上次歸還未滿30分鐘");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "歸還失敗");
          // u8g2_print_ch(0, 40, "上次歸還未滿30分鐘");
          // u8g2.sendBuffer();
          delay(3000);

          //last borrowed less than 30mins
        }
        else if (config.error_code == 4)
        {
          Serial.println("return rfid 4");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "歸還失敗");
          // u8g2_print_ch(0, 40, "商店杯子不足");
          // u8g2.sendBuffer();
          delay(3000);
          //cups in the store < 3
          //「 歸還失敗 」
          //「 商店杯子不足 」
        }
        else if (config.error_code == 5)
        {
          Serial.println("return rfid 5");
          oled.twolines("歸還失敗", "請先註冊帳號");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "歸還失敗");
          // u8g2_print_ch(0, 40, "請先綁定帳號");
          // u8g2.sendBuffer();
          delay(3000);
          // not verified
          //「 歸還失敗 」
          //「」
        }
      }
    }
    else if (barcode_work == 1)
    {
      Serial.print("qrcode: ");
      Serial.println(config.qrcode);
      int http_code = server.CupRecord(config.token, config.uid, "NFC", "uCup", "/do_return", config.error_code);
      if (http_code == 200)
      {
        Serial.println("return qrcode success");
        oled.twolines("歸還成功", "謝謝惠顧");
        // u8g2.clearBuffer();
        // u8g2_print_ch(0, 15, "歸還成功");
        // u8g2_print_ch(0, 40, "謝謝惠顧");
        // u8g2.sendBuffer();
        delay(3000);
        //success
        //TODO: print success text
      }
      else
      {
        if (config.error_code == 1)
        {
          Serial.println("return qrcode 1");
          oled.twolines("歸還失敗", "請先註冊會員");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "歸還失敗");
          // u8g2_print_ch(0, 40, "請先註冊會員");
          // u8g2.sendBuffer();
          delay(3000);
          //not registered
        }
        else if (config.error_code == 2 || config.error_code == 21)
        {
          Serial.println("return qrcode 2");
          oled.twolines("歸還失敗", "請先歸還杯子");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "歸還失敗");
          // u8g2_print_ch(0, 40, "請先歸還杯子");
          // u8g2.sendBuffer();
          delay(3000);
          //last borrowed cup not return
        }
        else if (config.error_code == 3)
        {
          Serial.println("return qrcode 3");
          oled.twolines("歸還失敗", "上次歸還未滿30分鐘");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "歸還失敗");
          // u8g2_print_ch(0, 40, "上次歸還未滿30分鐘");
          // u8g2.sendBuffer();
          delay(3000);
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
          delay(3000);
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
          delay(3000);
        }
      }
    }

    break;
  }
  }
}
