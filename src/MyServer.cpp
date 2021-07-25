#include "MyServer.h"
#include <Arduino.h>
#include "String.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "Config.h"
MyServer::MyServer() {}

String MyServer::GetToken()
{
    HTTPClient http;
    // Your Domain name with URL path or IP address with path
    http.begin(servername + "/stores/login");

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
    Serial.println("token:");
    Serial.println(token);
    return token;
}
int MyServer::CupBind(String token, String nfc_id, String ntu_id)
{
}
int MyServer::CupRecord(String token, String std_id, String provider, String type, String operation, int &errorcode)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;

        http.begin(servername + "/record" + operation);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        http.addHeader("Authorization", token);

        String httpRequestData = "user_id=" + std_id + "&provider=" + provider + "&cup_type=" + type;
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

        errorcode = doc["error_code"];
        Serial.print("error_code: ");
        Serial.println(errorcode);
        return httpResponseCode;
    }
    else
    {
        return 87;
    }
}

void MyServer::ErrHandle(int errorcode)
{
}