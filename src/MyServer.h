#pragma once
#include "Config.h"
#include "Arduino.h"
//#include "String.h"
class MyServer
{
public:
    MyServer();
    String GetToken();
    int CupRecord(String, String, String, String, String, int &);
    int CupBind(String, String, String);
    void ErrHandle(int);
    void testtoken(String &);

private:
};