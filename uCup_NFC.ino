#include "WiFi.h"
// #include <String.h>
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
// #include "./src/Barcode.h"
#include "./src/Buzzer.h"
#include "./src/LED.h"
#include "./src/MyServer.h"
// set WiFi name and password
#define EAP_IDENTITY "ID"       // if connecting from another corporation, use identity@organisation.domain in Eduroam
#define EAP_PASSWORD "PASSWORD" // your Eduroam password
const char *ssid = "WIFI_NAME";

#define RENT 0
#define RETURN 1
#define BIND_TIME 8000 // ms

char std_id_barcode[36];
const int BUZZ_PIN = 2;   // Buzzer pin
const int LED_RED = 27;   // Red LED pin
const int LED_GREEN = 13; // Blue LED pin
const int RST_PIN = 36;   // Reset pin
const int SS_PIN = 5;     // Slave select pin
// const String serverName = "https://ucup-dev.herokuapp.com/api";
// SoftwareSerial BarcodeScanner(12, 14); //rx,tx //barcode
// U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE); //OLED
Oled oled(U8G2_R0, SCL, SDA, U8X8_PIN_NONE);
Button button(17);
Config config;
SoftwareSerial barcode(14, 12);
Buzzer buzzer(BUZZ_PIN);
LED LED_R(LED_RED);
LED LED_G(LED_GREEN);
MyServer server;
Rfid rc522(19, 20);

bool testWifi(void)
{
  int c = 0;
  Serial.println("Waiting for Wifi to connect");
  oled.twolines_en("waiting for", "WiFi connection");
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

void LED_buzzer_success()
{
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(BUZZ_PIN, HIGH);
  delay(200);
  digitalWrite(BUZZ_PIN, LOW);
  delay(300);
  digitalWrite(LED_GREEN, LOW);
}
void LED_buzzer_success_b()
{
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(BUZZ_PIN, HIGH);
  delay(800);
  digitalWrite(BUZZ_PIN, LOW);
  delay(300);
  digitalWrite(LED_GREEN, LOW);
}
void LED_buzzer_fail()
{
  digitalWrite(LED_RED, HIGH);
  digitalWrite(BUZZ_PIN, HIGH);
  delay(200);
  digitalWrite(BUZZ_PIN, LOW);
  delay(200);
  digitalWrite(BUZZ_PIN, HIGH);
  delay(200);
  digitalWrite(BUZZ_PIN, LOW);
  delay(300);
  digitalWrite(LED_RED, LOW);
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
  barcode.begin(9600);
  Serial.print("Connecting to network: ");
  // Serial.println(config.ssid);
  WiFi.disconnect(true);
  // for normal WiFi
  // WiFi.begin(config.ssid, config.passphrase);

  // for NTU peap                                                             //disconnect form wifi to set new wifi connection
  WiFi.mode(WIFI_STA);                                                               // init wifi mode
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY)); // provide identity
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY)); // provide username --> identity and username is same
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD)); // provide password
  esp_wpa2_config_t esp_config = WPA2_CONFIG_INIT_DEFAULT();                         // set config settings to default
  esp_wifi_sta_wpa2_ent_enable(&esp_config);                                         // set config settings to enable function
  WiFi.begin(ssid);
  // connect to wifi
  int counter = 0;
  oled.twolines_en("Store ID:", config.Account);
  delay(2000);
  oled.printe(0, 15, "WiFi loading...");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    counter++;
    if (counter >= 60)
    { // after 30 seconds timeout - reset board
      ESP.restart();
    }
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address set: ");
  Serial.println(WiFi.localIP()); // print LAN IP

  if (testWifi())
  {
    oled.printe(15, 30, "WiFi OK");
    Serial.println("WiFi connected OK");
    Serial.print("Local IP: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    // log WiFi error
    oled.printe(15, 30, "WiFi error");
    LED_buzzer_fail();
    Serial.println("WiFi connected NG");
  }
  SPI.begin();                     // Init SPI bus
  rc522.PCD_Init();                // Init MFRC522
  rc522.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
  delay(2000);
  LED_G.blink(1000);
  LED_R.blink(1000);
  oled.printc(40, 40, "載入中");
  config.token = server.GetToken(config.Account, config.Password);
  Serial.println("config.token:");
  Serial.println(config.token);
  if (config.token == "err")
  {
    oled.twolines_en("error", "Please Restart");
    LED_buzzer_fail();
  }
  // oled.printe(15, 15, "Touch to Start");
  else
  {
    oled.twolines_en("Welcome uCup", "Touch to start");
    LED_buzzer_success();
  }
}

void loop()
{
  // manage the operation mode
  button.click(config.button_cnt, config.success);
  switch (config.button_cnt % 2)
  {
  case RENT:
  {
    // rent
    oled.twolines("借用模式", "請感應學生證");
    config.rfid_work = rc522.detect(config.uid);
    config.barcode_work = detect_barcode(36, config.qrcode);
    // show rent message
    if (config.rfid_work == 1 && config.uid.length() == 8)
    {
      Serial.println("len:");
      Serial.println(config.uid.length());
      oled.printc(40, 40, "載入中");

      Serial.println("uid:");
      Serial.println(rc522.detect(config.uid));
      int http_code = server.CupRecord(config.token, config.uid, "NFC", "uCup", "/do_rent", config.error_code);
      if (http_code == 200)
      {
        oled.twolines("借用成功", "謝謝惠顧");
        Serial.println("rent 200");
        finish();
        LED_buzzer_success();
        delay(1000);
      }
      else if (http_code == 877)
      {
        oled.twolines_en("Please Check", "WiFi Connection");
        LED_buzzer_fail();
      }
      else
      {
        if (config.error_code == 1)
        {
          oled.twolines("借用失敗", "請先註冊帳號");
          // delay(2000);
          // oled.twolines_en("Not yet registered", "Please register first");
          Serial.println("rent rfid 1");
          LED_buzzer_fail();
          delay(2000);
          Serial.println("direct to bind mode");

          oled.twolines("綁定模式", "請掃描學生證條碼");
          int bind_start_time = millis();
          int bind_http_code = 0;
          while (millis() - bind_start_time < BIND_TIME)
          {
            int bind_work = detect_barcode(9, config.stdID);
            if (bind_work == 1)
            {
              oled.printc(40, 40, "載入中");
              bind_http_code = server.CupBind(config.token, config.uid, config.stdID);
              break;
            }
          }
          if (bind_http_code == 200)
          {
            oled.twolines("綁定成功", "請重新操作");
            Serial.println("bind success");
            finish();
            LED_buzzer_success_b();
          }
          else if (http_code == 877)
          {
            oled.twolines_en("Please Check", "WiFi Connection");
            LED_buzzer_fail();
          }
          else
          {

            oled.twolines("綁定失敗", "請重新操作");
            Serial.println("bind fail");
            finish();
            LED_buzzer_fail();
          }
          delay(2000);
        }
        else if (config.error_code == 2 || config.error_code == 21)
        {
          Serial.println("rent rfid 2");
          oled.twolines("借用失敗", "請先歸還杯子");
          finish();
          LED_buzzer_fail();
          delay(1000);
        }
        else if (config.error_code == 3)
        {
          Serial.println("rent rfid 3");
          oled.twolines("借用失敗", "上次借用未滿30分鐘");
          finish();
          LED_buzzer_fail();
          delay(1000);
        }
        else if (config.error_code == 4)
        {
          Serial.println("rent rfid 4");
          oled.twolines("借用失敗", "商店杯子不足");
          finish();
          LED_buzzer_fail();
          delay(1000);
        }
        else if (config.error_code == 5)
        {
          Serial.print("rent 5");
          oled.twolines("借用失敗", "請先註冊帳號");
          finish();
          LED_buzzer_fail();
          delay(1000);
        }
        else if (config.error_code == 6)
        {
          Serial.println("rent qrcode 6");
          oled.twolines_en("Authority", "Fail");
          finish();
          LED_buzzer_fail();
          delay(1000);
        }
      }
    }
    else if (config.barcode_work == 1 && config.qrcode.length() == 36)
    {
      oled.printc(40, 40, "載入中");
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
        oled.twolines("借用成功", "謝謝惠顧");
        finish();
        LED_buzzer_success();
        delay(1000);
      }
      else if (http_code == 877)
      {
        oled.twolines_en("Please Check", "WiFi Connection");
        LED_buzzer_fail();
      }
      else
      {
        if (config.error_code == 1)
        {
          Serial.println("rent qrcode 1");
          oled.twolines("借用失敗", "請先註冊會員");
          finish();
          LED_buzzer_fail();
          delay(1000);
        }
        else if (config.error_code == 2 || config.error_code == 21)
        {
          Serial.println("rent qrcode 2");
          oled.twolines("借用失敗", "請先歸還杯子");
          finish();
          LED_buzzer_fail();
          delay(1000);
        }
        else if (config.error_code == 3)
        {
          Serial.println("rent qrcode 3");
          oled.twolines("借用失敗", "上次租借未滿30分鐘");
          finish();
          LED_buzzer_fail();
          delay(1000);
        }
        else if (config.error_code == 4)
        {
          Serial.println("rent qrcode 4");
          oled.twolines("借用失敗", "商店杯子不足");
          finish();
          LED_buzzer_fail();
          delay(1000);
        }
        else if (config.error_code == 5)
        {
          Serial.println("rent qrcode 5");
          oled.twolines("借用失敗", "請先綁定帳號");
          finish();
          LED_buzzer_fail();
          delay(1000);
        }
        else if (config.error_code == 6)
        {
          Serial.println("rent qrcode 6");
          oled.twolines_en("Authority", "Fail");
          finish();
          LED_buzzer_fail();
          delay(1000);
        }
      }
    }
    break;
  }
  case RETURN:
  {
    // return
    config.rfid_work = rc522.detect(config.uid);
    config.barcode_work = detect_barcode(36, config.qrcode);
    oled.twolines("歸還模式", "請感應學生證");
    // show return message
    if (config.rfid_work == 1 && config.uid.length() == 8)
    {
      oled.printc(35, 40, "載入中");
      Serial.print("uid: ");
      Serial.println(config.uid);
      int http_code = server.CupRecord(config.token, config.uid, "NFC", "uCup", "/do_return", config.error_code);
      if (http_code == 200)
      {
        oled.twolines("歸還成功", "謝謝惠顧");
        Serial.println("return 200");
        LED_buzzer_success();
        finish();
        delay(1000);
      }
      else if (http_code == 877)
      {
        oled.twolines_en("Please Check", "WiFi Connection");
        LED_buzzer_fail();
      }
      else
      {
        if (config.error_code == 1)
        {
          Serial.println("return rfid 1");
          oled.twolines("歸還失敗", "請先註冊會員");
          LED_buzzer_fail();
          Serial.println("direct to bind mode");
          oled.twolines("綁定模式", "請掃描學生證條碼");
          int bind_start_time = millis();
          int bind_http_code = 0;
          while (millis() - bind_start_time < BIND_TIME)
          {
            int bind_work = detect_barcode(9, config.stdID);
            if (bind_work == 1)
            {
              oled.printc(40, 40, "載入中");
              bind_http_code = server.CupBind(config.token, config.uid, config.stdID);
              break;
            }
          }
          if (bind_http_code == 200)
          {
            oled.twolines("綁定成功", "請重新操作");
            Serial.println("bind success");
            finish();
            LED_buzzer_success_b();
          }
          else if (http_code == 877)
          {
            oled.twolines_en("Please Check", "WiFi Connection");
            LED_buzzer_fail();
          }
          else
          {
            oled.twolines("綁定失敗", "請重新操作");
            Serial.println("bind fail");
            finish();
            LED_buzzer_fail();
          }
          delay(1000);
        }
        else if (config.error_code == 2 || config.error_code == 21)
        {
          Serial.println("return rfid 2");
          oled.twolines("歸還失敗", "請先歸還杯子");
          finish();
          LED_buzzer_fail();
          delay(1000);
        }
        else if (config.error_code == 3)
        {
          Serial.println("return rfid 3");
          oled.twolines("歸還失敗", "上次歸還未滿30分鐘");
          finish();
          LED_buzzer_fail();
          delay(1000);
        }
        else if (config.error_code == 4)
        {
          Serial.println("return rfid 4");
          oled.twolines("歸還失敗", "商店杯子不足");
          finish();
          LED_buzzer_fail();
          delay(1000);
        }
        else if (config.error_code == 5)
        {
          Serial.println("return rfid 5");
          oled.twolines("歸還失敗", "請先註冊帳號");
          finish();
          LED_buzzer_fail();
          delay(1000);
        }
        else if (config.error_code == 6)
        {
          Serial.println("rent qrcode 6");
          oled.twolines_en("Authority", "Fail");
          finish();
          LED_buzzer_fail();
          delay(1000);
        }
        else if (config.error_code == 9)
        {
          Serial.println("return rfid 9");
          oled.twolines("歸還失敗", "請先借用杯子");
          finish();
          LED_buzzer_fail();
          delay(1000);
        }
      }
    }
    else if (config.barcode_work == 1 && config.qrcode.length() == 36)
    {
      oled.printc(40, 40, "載入中");
      Serial.print("qrcode: ");
      Serial.println(config.qrcode);
      int http_code = server.CupRecord(config.token, config.qrcode, "Normal", "uCup", "/do_return", config.error_code);
      if (http_code == 200)
      {
        Serial.println("return qrcode success");
        oled.twolines("歸還成功", "謝謝惠顧");
        finish();
        LED_buzzer_success();
        delay(1000);
      }
      else if (http_code == 877)
      {
        oled.twolines_en("Please Check", "WiFi Connection");
        LED_buzzer_fail();
      }
      else
      {
        if (config.error_code == 1)
        {
          Serial.println("return qrcode 1");
          oled.twolines("歸還失敗", "請先註冊會員");
          finish();
          LED_buzzer_fail();
          delay(1000);
        }
        else if (config.error_code == 2 || config.error_code == 21)
        {
          Serial.println("return qrcode 2");
          oled.twolines("歸還失敗", "請先歸還杯子");
          finish();
          LED_buzzer_fail();
          delay(1000);
        }
        else if (config.error_code == 3)
        {
          Serial.println("return qrcode 3");
          oled.twolines("歸還失敗", "上次歸還未滿30分鐘");
          finish();
          LED_buzzer_fail();
          delay(1000);
        }
        else if (config.error_code == 4)
        {
          Serial.println("return qrcode 4");
          oled.twolines("歸還失敗", "商店杯子不足");
          finish();
          LED_buzzer_fail();
          delay(1000);
        }
        else if (config.error_code == 5)
        {
          Serial.println("return qrcode 5");
          oled.twolines("歸還失敗", "請先註冊帳號");
          finish();
          LED_buzzer_fail();
          delay(1000);
        }
        else if (config.error_code == 6)
        {
          Serial.println("rent qrcode 6");
          oled.twolines_en("Authority", "Fail");
          finish();
          LED_buzzer_fail();
          delay(1000);
        }
        else if (config.error_code == 9)
        {
          Serial.println("return rfid 9");
          oled.twolines("歸還失敗", "請先借用杯子");
          finish();
          LED_buzzer_fail();
          delay(1000);
        }
      }
    }
    break;
  }
  }
}
