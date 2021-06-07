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
//set WiFi name and password
char *ssid = "carlos";
char *passphrase = "carlosyoyo";

#define RENT 0
#define RETURN 1

const int RST_PIN = 36;                                                                               // Reset pin
const int SS_PIN = 5;                                                                                 // Slave select pin
const String serverName = "https://ucup-dev.herokuapp.com/api";
SoftwareSerial BarcodeScanner(12, 14); //rx,tx
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
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
int button_ctn;
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
    StaticJsonDocument<300> doc;
    int httpResponseCode = http.POST(httpRequestData);
    String res = http.getString();
    DeserializationError error = deserializeJson(doc, res);

    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return 87;
    }
    
    error_code = doc["error_code"];
    Serial.print("cup_httpcode: ");
    Serial.println(httpResponseCode);
    Serial.print("error_code: ");
    Serial.println(error_code);
    return httpResponseCode;
  }
  else
  {
    return 87;
  }
}
int cup_bind(String token, String nfc_id, String ntu_id){
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
void u8g2_print_en(int x, int y, String text){
  u8g2.setFont(u8g2_font_unifont_t_chinese2);
  u8g2.setFontDirection(0);
  u8g2.setCursor(x,y);
  u8g2.print(text);
  u8g2.sendBuffer();
}
void u8g2_print_ch(int x, int y, String text){
  u8g2.setFont(u8g2_font_unifont_t_chinese1);
  u8g2.setFontDirection(0);
  u8g2.setCursor(x,y);
  u8g2.print(text);
  u8g2.sendBuffer();
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
  //begin OLED
  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.clearBuffer();
  u8g2_print_en(15,15,"Welcome");
  u8g2_print_en(15,40,"uCup");
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
  u8g2.clearBuffer();
}

void loop()
{
  String token = gettoken();
  // manage the operation mode
  Serial.print("Token: ");
  Serial.println(token);
  int button = digitalRead(17);
  if (button == 0){
    button_ctn += 1;
    success = false;
  }
  

  switch (button_ctn % 2)
  {
  case 0:{
    //rent
    int start_rent_time = 0;
    start_rent_time = millis();
    u8g2.clearBuffer();
    u8g2_print_en(15,15,"rent");

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
          //success
          //TODO: print success text
          //「 租借成功 」
          //「 謝謝惠顧 」
          
          success = true;
        }
        else{
            if (error_code == 1)
            {
              //not registered
              //「 租借失敗 」
              //「 請先註冊uCup會員 」
            }
            else if (error_code == 2 || error_code == 21)
            {
              //last borrowed cup not return
              //「 租借失敗 」
              //「 請先歸還杯子 」


            }
            else if (error_code == 3)
            {
              //last borrowed less than 30mins
              //「 租借失敗 」
              //「 上次租借未滿30分鐘」
            }
            else if (error_code == 4)
            {
              //cups in the store < 3
              //「 租借失敗 」
              //「 商店杯子不足 」

            }
            else if (error_code == 5)
            {
              // not verified
              //「 租借失敗 」
              //「請先綁定帳號」
              delay(500);

              //register
              //「請先註冊」
              //「請先感應學生證」
              if(detect_rfid() == 1){
                //「請掃描學生證條碼」
                if(detect_scan_qrcode() == 1){
                }

              }
            }
          }
      }
    else if (detect_scan_qrcode() == 1)
    {
      Serial.print("stdid: ");
      Serial.println(stdid);
      int http_code = cup_record(token, uid, "NFC", "uCup", "/do_rent");
      if (http_code == 200)
      {
        //success
        //TODO: print success text
        //「 租借成功 」
        //「 謝謝惠顧 」
      
      }
      else{
          if (error_code == 1)
          {
            //not registered
            //「 租借失敗 」
            //「 請先註冊uCup會員 」

          }
          else if (error_code == 2 || error_code == 21)
          {
            //last borrowed cup not return
            //「 租借失敗 」
            //「 請先歸還杯子 」

          }
          else if (error_code == 3)
          {
            //last borrowed less than 30mins
            //「 租借失敗 」
            //「 上次租借未滿30分鐘」
          }
          else if (error_code == 4)
          {
            //cups in the store < 3
            //「 租借失敗 」
            //「 商店杯子不足 」
          }
          else if (error_code == 5)
          {
            // not verified
            //「 租借失敗 」
            //「請先綁定帳號」
            delay(500);

            //register
            //「請先註冊」
            //「請先感應學生證」
            if(detect_rfid() == 1){
              //「請掃描學生證條碼」
              if(detect_scan_qrcode() == 1){
              }

            }


          }
      }
    }
  break;
  }
  case 1:{
    //return
    int start_return_time = 0;
    start_return_time = millis();
    u8g2.clearBuffer();
    u8g2_print_en(15,15,"return");
//      if (button == 0){
//        button_ctn += 1;
//        success = false;
//      }
      //show return message
      if (detect_rfid() == 1)
      {
        Serial.print("uid: ");
        Serial.println(uid);
        int error_code = cup_record(token, uid, "NFC", "uCup", "./do_rent");
        if (error_code == 0)
        {
          //success
          //TODO: print success text
          //「 租借成功 」
          //「 謝謝惠顧 」
          

        }
        else if (error_code == 1)
        {
          //not registered
          //「 租借失敗 」
          //「 請先註冊uCup會員 」

        }
        else if (error_code == 2 || error_code == 21)
        {
          //last borrowed cup not return
          //「 租借失敗 」
          //「 請先歸還杯子 」

        }
        else if (error_code == 3)
        {
          //last borrowed less than 30mins
          //「 租借失敗 」
          //「 上次租借未滿30分鐘」
        }
        else if (error_code == 4)
        {
          //cups in the store < 3
          //「 租借失敗 」
          //「 商店杯子不足 」
        }
        else if (error_code == 5)
        {
          // not verified
          //「 租借失敗 」
          //「」
        }
      }
      else if (detect_scan_qrcode() == 1)
      {
        Serial.print("stdid: ");
        Serial.println(stdid);
        int http_code = cup_record(token, uid, "NFC", "uCup", "/do_rent");
        if (http_code == 200)
        {
          //success
          //TODO: print success text
          //「 租借成功 」
          //「 謝謝惠顧 」
          

        }
        else{
            if (error_code == 1)
            {
              //not registered
              //「 租借失敗 」
              //「 請先註冊uCup會員 」

  
            }
            else if (error_code == 2 || error_code == 21)
            {
              //last borrowed cup not return
              //「 租借失敗 」
              //「 請先歸還杯子 」

  
            }
            else if (error_code == 3)
            {
              //last borrowed less than 30mins
              //「 租借失敗 」
              //「 上次租借未滿30分鐘」
            }
            else if (error_code == 4)
            {
              //cups in the store < 3
              //「 租借失敗 」
              //「 商店杯子不足 」
  
            }
            else if (error_code == 5)
            {
              // not verified
              //「 租借失敗 」
              //「請先綁定帳號」
              delay(500);

              //register
              //「請先註冊」
              //「請先感應學生證」
              if(detect_rfid() == 1){
                //「請掃描學生證條碼」
                if(detect_scan_qrcode() == 1){
                }

              }

            }
        }
      }
    
    break;
  }
  }
}
