#include "LED.h"
#include <Arduino.h>

//constructor
LED::LED(int pin)
{
    _pin = pin;
    pinMode(_pin, OUTPUT);
}

//blink(int BlinkSec) //ms
void LED::blink(int ms)
{
    digitalWrite(_pin, HIGH);
    delay(ms);
    digitalWrite(_pin, LOW);
}
