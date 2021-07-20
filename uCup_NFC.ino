#include "SoftwareSerial.h"
#include "WiFi.h"
#include <String.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include "./src/Rfid.h"
#include "./src/Oled.h"
//set WiFi name and password
char *ssid = "carlos";
char *passphrase = "carlosyoyo";

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
const String serverName = "https://ucup-dev.herokuapp.com/api";
SoftwareSerial BarcodeScanner(12, 14); //rx,tx //barcode
//U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE); //OLED
Oled oled(U8G2_R0, SCL, SDA);
byte uidd[4];
char uid_char[9];
String uid;
char std_id_barcode[9];
char qrcode_barcode[36];
String stdid;
String qrcode;
bool is_card_valid = false;
int error_code = -1;
bool success = false;
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance
//Rfid mfrc522(SS_PIN, RST_PIN);
int button_ctn;
int button_init = 0;
int lastButtonState = 0;
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
void array_to_string(byte array[], unsigned int len, char buffer[])
{
  for (unsigned int i = 0; i < len; i++)
  {
    byte nib1 = (array[i] >> 4) & 0x0F;
    byte nib2 = (array[i] >> 0) & 0x0F;
    buffer[i * 2 + 0] = nib1 < 0xA ? '0' + nib1 : 'A' + nib1 - 0xA;
    buffer[i * 2 + 1] = nib2 < 0xA ? '0' + nib2 : 'A' + nib2 - 0xA;
  }
  buffer[len * 2] = '\0';
}
int detect_rfid()
{
  int res = 0;
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
  {
    return 0;
  }
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K)
  {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return 0;
  }
  if (mfrc522.uid.uidByte[0] != uid[0] ||
      mfrc522.uid.uidByte[1] != uid[1] ||
      mfrc522.uid.uidByte[2] != uid[2] ||
      mfrc522.uid.uidByte[3] != uid[3])
  {
    res = 1;
    for (byte i = 0; i < 4; i++)
    {
      uidd[i] = mfrc522.uid.uidByte[i];
    }
    Serial.print("uid: ");
    array_to_string(uidd, 4, uid_char);
    String uid_tmp(uid_char);
    uid = uid_tmp;
    //test uid
    Serial.print(uid);
  }
  else
  {
    //same card
    res = 2;
  }
  mfrc522.PICC_HaltA();
  return res;
}
int detect_scan_std()
{
  int index = 0;
  if (BarcodeScanner.available())
  {
    while (index < 9)
    {
      std_id_barcode[index] = (char)(BarcodeScanner.read());
      index++;
    }
    std_id_barcode[index] = '\0';
    String stdid_tmp(std_id_barcode);
    stdid = stdid_tmp;
    return 1;
  }
  return 0;
}
int detect_scan_qrcode()
{
  int index = 0;
  if (BarcodeScanner.available())
  {
    while (index < 36)
    {
      qrcode_barcode[index] = (char)(BarcodeScanner.read());
      index++;
    }
    std_id_barcode[index] = '\0';
    String qrcode_tmp(qrcode_barcode);
    qrcode = qrcode_tmp;
    return 1;
  }
  return 0;
}
String gettoken()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;

    // Your Domain name with URL path or IP address with path
    http.begin(serverName + "/stores/login");

    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    // Data to send with HTTP POST
    String httpRequestData = "phone=0900000000&password=choosebetterbebetter";
    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);
    StaticJsonDocument<900> doc;
    DeserializationError error = deserializeJson(doc, http.getString());

    // log err
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return "0";
    }

    //parse the data to get token
    String token = doc["token"];
    token = "Bearer " + token;
    return token;
  }
  else
  {
    return "WIFI disconnected.";
  }
}
int cup_record(String token, String stdID, String provider, String type, String operation)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;

    http.begin(serverName + "/record" + operation);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("Authorization", token);

    String httpRequestData = "user_id=" + stdID + "&provider=" + provider + "&cup_type=" + type;
    //log info of http req
    Serial.println(serverName + "/record" + operation);
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

    error_code = doc["error_code"];
    Serial.print("error_code: ");
    Serial.println(error_code);
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

    http.begin(serverName + "/users/bind_ntu_nfc");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("Authorization", token);

    String httpRequestData = "nfc_id=" + nfc_id + "&ntu_id=" + ntu_id;
    //log info of http req
    Serial.println(serverName + "/users/bind_ntu_nfc");
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
// void u8g2_print_en(int x, int y, String text)
// {
//   u8g2.setFont(u8g2_font_unifont_t_chinese2);
//   u8g2.setFontDirection(0);
//   u8g2.setCursor(x, y);
//   u8g2.print(text);
//   u8g2.sendBuffer();
// }
// void u8g2_print_ch(int x, int y, String text)
// {
//   u8g2.setFont(u8g2_font_unifont_t_chinese1);
//   u8g2.setFontDirection(0);
//   u8g2.setCursor(x, y);
//   u8g2.print(text);
//   //u8g2.sendBuffer();
// }
void buzz()
{
}
void setup()
{
  Serial.begin(9600); // Initialize serial communications with the PCSerial.begin(9600);
  //begin WiFi
  WiFi.begin(ssid, passphrase);
  //begin barcode
  BarcodeScanner.begin(9600);
  //pin for button
  pinMode(17, INPUT_PULLUP);
  //init button_ctn
  button_ctn = -1;
  //init Buzzer
  pinMode(BUZZ_PIN, OUTPUT);
  //init LED
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  //begin OLED
  // u8g2.begin();
  // u8g2.enableUTF8Print();
  // u8g2.clearBuffer();
  // u8g2_print_en(15, 15, "Welcome");
  // u8g2_print_en(15, 40, "uCup");
  oled.print_en(15, 15, "Welcome");

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
  SPI.begin();                       // Init SPI bus
  mfrc522.PCD_Init();                // Init MFRC522
  mfrc522.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
  delay(2000);
  //u8g2.clearBuffer();
  oled.clearbuf();
  button_init = digitalRead(17);
  token = gettoken();
}

void loop()
{
  // manage the operation mode
  // Serial.print("Token: ");
  // Serial.println(token);
  int button = digitalRead(17);
  if (button == 0)
  {
    button_ctn += 1;
    success = false;
  }
  switch (button_ctn % 2)
  {
  case RENT:
  {
    //rent
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
    if (detect_rfid() == 1)
    {
      Serial.print("uid: ");
      Serial.println(uid);
      int http_code = cup_record(token, uid, "NFC", "uCup", "/do_rent");
      if (http_code == 200)
      {
        // u8g2.clearBuffer();
        // u8g2_print_ch(0, 15, "租借成功");
        // u8g2_print_ch(0, 40, "謝謝惠顧");
        // u8g2.sendBuffer();
        oled.twolines("租借成功", "謝謝惠顧");
        Serial.println("rent 200");
        //success
        //TODO: print success text
        //「 租借成功 」
        //「 謝謝惠顧 」

        success = true;
        delay(3000);
      }
      else
      {
        if (error_code == 1)
        {
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "租借失敗");
          // u8g2_print_ch(0, 40, "請先註冊uCup會員");
          // u8g2.sendBuffer();

          //not registered
          //「 租借失敗 」
          //「 請先註冊uCup會員 」
          Serial.println("rent rfid 1");
          delay(2000);
          Serial.println("direct to bind mode");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "綁定模式");
          // u8g2_print_ch(0, 40, "請掃描學生證條碼");
          // u8g2.sendBuffer();
          int bind_start_time = millis();
          int bind_http_code = 0;
          while (millis() - bind_start_time < BIND_TIME)
          {
            if (detect_scan_std() == 1)
            {
              bind_http_code = cup_bind(token, uid, stdid);
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
            Serial.println("bind success");
            success = true;
          }
          else
          {
            // u8g2.clearBuffer();
            // u8g2_print_ch(0, 15, "綁定失敗");
            // u8g2_print_ch(0, 40, "請重新操作");
            // u8g2.sendBuffer();
            Serial.println("bind fail");
            success = true;
          }
          delay(3000);
        }
        else if (error_code == 2 || error_code == 21)
        {
          Serial.println("rent rfid 2");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "租借失敗");
          // u8g2_print_ch(0, 40, "請先歸還杯子");
          // u8g2.sendBuffer();
          //last borrowed cup not return
          //「 租借失敗 」
          //「 請先歸還杯子 」
          delay(3000);
          success = true;
        }
        else if (error_code == 3)
        {
          Serial.println("rent rfid 3");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "租借失敗");
          // u8g2_print_ch(0, 40, "上次租借未滿30分鐘");
          // u8g2.sendBuffer();
          //last borrowed less than 30mins
          //「 租借失敗 」
          //「 上次租借未滿30分鐘」
          delay(3000);
        }
        else if (error_code == 4)
        {
          Serial.println("rent rfid 4");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "租借失敗");
          // u8g2_print_ch(0, 40, "商店杯子不足");
          // u8g2.sendBuffer();
          //cups in the store < 3
          //「 租借失敗 」
          //「 商店杯子不足 」
          delay(3000);
        }
        else if (error_code == 5)
        {
          Serial.print("rent 5");
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
    else if (detect_scan_qrcode() == 1)
    {
      Serial.print("qrcode: ");
      Serial.println(qrcode);
      int http_code = cup_record(token, qrcode, "Normal", "uCup", "/do_rent");
      if (http_code == 200)
      {
        Serial.println("rent qrcode success");
        // u8g2.clearBuffer();
        // u8g2_print_ch(0, 15, "租借成功");
        // u8g2_print_ch(0, 40, "謝謝惠顧");
        // u8g2.sendBuffer();
        success = true;
        delay(3000);
        //success
        //TODO: print success text
        //「 租借成功 」
        //「 謝謝惠顧 」
      }
      else
      {
        if (error_code == 1)
        {
          Serial.println("rent qrcode 1");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "租借失敗");
          // u8g2_print_ch(0, 40, "請先註冊會員");
          // u8g2.sendBuffer();
          delay(3000);
          //not registered
          //「 租借失敗 」
          //「 請先註冊uCup會員 」
        }
        else if (error_code == 2 || error_code == 21)
        {
          Serial.println("rent qrcode 2");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "租借失敗");
          // u8g2_print_ch(0, 40, "請先歸還杯子");
          // u8g2.sendBuffer();
          delay(3000);
          //last borrowed cup not return
          //「 租借失敗 」
          //「 請先歸還杯子 」
        }
        else if (error_code == 3)
        {
          Serial.println("rent qrcode 3");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "租借失敗");
          // u8g2_print_ch(0, 40, "上次租借未滿30分鐘");
          // u8g2.sendBuffer();
          delay(3000);
          //last borrowed less than 30mins
          //「 租借失敗 」
          //「 上次租借未滿30分鐘」
        }
        else if (error_code == 4)
        {
          Serial.println("rent qrcode 4");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "租借失敗");
          // u8g2_print_ch(0, 40, "商店杯子不足");
          // u8g2.sendBuffer();
          delay(3000);
          //cups in the store < 3
          //「 租借失敗 」
          //「 商店杯子不足 」
        }
        else if (error_code == 5)
        {
          Serial.println("rent qrcode 5");
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
    if (detect_rfid() == 1)
    {
      Serial.print("uid: ");
      Serial.println(uid);
      int http_code = cup_record(token, uid, "NFC", "uCup", "./do_return");
      if (http_code == 200)
      {
        Serial.println("return rfid success");
        // u8g2.clearBuffer();
        // u8g2_print_ch(0, 15, "歸還成功");
        // u8g2_print_ch(0, 40, "謝謝惠顧");
        // u8g2.sendBuffer();

        Serial.println("direct to bind mode");
        // u8g2.clearBuffer();
        // u8g2_print_ch(0, 15, "綁定模式");
        // u8g2_print_ch(0, 40, "請掃描學生證條碼");
        // u8g2.sendBuffer();
        int bind_start_time = millis();
        int bind_http_code = 0;
        while (millis() - bind_start_time < BIND_TIME)
        {
          if (detect_scan_std() == 1)
          {
            bind_http_code = cup_bind(token, uid, stdid);
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
          Serial.println("bind success");
          success = true;
        }
        else
        {
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "綁定失敗");
          // u8g2_print_ch(0, 40, "請重新操作");
          // u8g2.sendBuffer();
          Serial.println("bind fail");
          success = true;
        }
        delay(3000);
        //success
        //TODO: print success text
        //「 歸還成功 」
        //「 謝謝惠顧 」
      }
      else
      {
        if (error_code == 1)
        {
          Serial.println("return rfid 1");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "歸還失敗");
          // u8g2_print_ch(0, 40, "請先註冊會員");
          // u8g2.sendBuffer();
          delay(3000);
          //not registered
          //「 歸還失敗 」
          //「 請先註冊uCup會員 」
        }
        else if (error_code == 2 || error_code == 21)
        {
          Serial.println("return rfid 2");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "歸還失敗");
          // u8g2_print_ch(0, 40, "請先歸還杯子");
          // u8g2.sendBuffer();
          delay(3000);
          //last borrowed cup not return
          //「 歸還失敗 」
          //「 請先歸還杯子 」
        }
        else if (error_code == 3)
        {
          Serial.println("return rfid 3");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "歸還失敗");
          // u8g2_print_ch(0, 40, "上次歸還未滿30分鐘");
          // u8g2.sendBuffer();
          delay(3000);

          //last borrowed less than 30mins
          //「 歸還失敗 」
          //「 上次租借未滿30分鐘」
        }
        else if (error_code == 4)
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
        else if (error_code == 5)
        {
          Serial.println("return rfid 5");
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
    else if (detect_scan_qrcode() == 1)
    {
      Serial.print("qrcode: ");
      Serial.println(qrcode);
      int http_code = cup_record(token, uid, "NFC", "uCup", "/do_return");
      if (http_code == 200)
      {
        Serial.println("return qrcode success");
        // u8g2.clearBuffer();
        // u8g2_print_ch(0, 15, "歸還成功");
        // u8g2_print_ch(0, 40, "謝謝惠顧");
        // u8g2.sendBuffer();
        delay(3000);
        //success
        //TODO: print success text
        //「 歸還成功 」
        //「 謝謝惠顧 」
      }
      else
      {
        if (error_code == 1)
        {
          Serial.println("return qrcode 1");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "歸還失敗");
          // u8g2_print_ch(0, 40, "請先註冊會員");
          // u8g2.sendBuffer();
          delay(3000);
          //not registered
          //「 歸還失敗 」
          //「 請先註冊uCup會員 」
        }
        else if (error_code == 2 || error_code == 21)
        {
          Serial.println("return qrcode 2");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "歸還失敗");
          // u8g2_print_ch(0, 40, "請先歸還杯子");
          // u8g2.sendBuffer();
          delay(3000);
          //last borrowed cup not return
          //「 歸還失敗 」
          //「 請先歸還杯子 」
        }
        else if (error_code == 3)
        {
          Serial.println("return qrcode 3");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "歸還失敗");
          // u8g2_print_ch(0, 40, "上次歸還未滿30分鐘");
          // u8g2.sendBuffer();
          delay(3000);
          //last borrowed less than 30mins
          //「 歸還失敗 」
          //「 上次租借未滿30分鐘」
        }
        else if (error_code == 4)
        {
          Serial.println("return qrcode 4");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "歸還失敗");
          // u8g2_print_ch(0, 40, "商店杯子不足");
          // u8g2.sendBuffer();
          delay(3000);
          //cups in the store < 3
          //「 歸還失敗 」
          //「 商店杯子不足 」
        }
        else if (error_code == 5)
        {
          Serial.println("return qrcode 5");
          // u8g2.clearBuffer();
          // u8g2_print_ch(0, 15, "歸還失敗");
          // u8g2_print_ch(0, 40, "請先綁定帳號");
          // u8g2.sendBuffer();
          // not verified
          //「 歸還失敗 」
          //「請先綁定帳號」
          delay(3000);
        }
      }
    }

    break;
  }
  }
}
