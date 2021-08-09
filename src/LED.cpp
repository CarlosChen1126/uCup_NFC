#include "LED.h"
#include <Arduino.h>

//constructor
LED::LED(int pin) : _pin(pin)
{
    pinMode(_pin, OUTPUT);
    Serial.println("LED ready");
}

//blink(int BlinkSec) //ms
void LED::blink(int ms)
{
    int start_time = millis();
    digitalWrite(_pin, HIGH);
    if (millis() - start_time > ms)
    {
        digitalWrite(_pin, LOW);
    }
}
