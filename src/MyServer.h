#pragma once
#include "Config.h"
#include "Arduino.h"
#include "String.h"
class MyServer
{
public:
    MyServer();
    String GetToken(String);
    int CupRecord(String, String, String, String, String);
    int CupBind(String, String, String);

private:
};