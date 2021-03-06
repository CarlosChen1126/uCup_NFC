#include "Buzzer.h"
#include <Arduino.h>

//constructor
Buzzer::Buzzer(int pin) : _pin(pin)
{
    pinMode(_pin, OUTPUT);
}

// ring the buzzer(int BuzzSecond) //ms
void Buzzer::buzz(int ms)
{
    digitalWrite(_pin, HIGH);
    delay(ms);
    digitalWrite(_pin, LOW);
    // tone(_pin, 1000);
    // delay(ms);
    // noTone(_pin);
}
int Buzzer::getPin()
{
    return _pin;
}