#include "MyServer.h"
#include <Arduino.h>
#include "String.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "Config.h"
MyServer::MyServer() {}

String MyServer::GetToken(String servername)
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
int MyServer::CupRecord(String token, String std_id, String provider, String type, String operation)
{
}