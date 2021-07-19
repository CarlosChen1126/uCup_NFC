#include "Buzzer.h"
#include <Arduino.h>

//constructor
Buzzer::Buzzer(int pin)
{
    _pin = pin;
    pinMode(_pin, OUTPUT);
}

// ring the buzzer(int BuzzSecond) //ms
Buzzer::buzz(int ms)
{
    digitalWrite(_pin, HIGH);
    delay(ms);
    digitalWrite(_pin, LOW);
}