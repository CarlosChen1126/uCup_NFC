#include "Button.h"
#include <Arduino.h>
#include "Config.h"

Button::Button(int pin) : _pin(pin)
{
    pinMode(_pin, INPUT_PULLUP);
}

void Button::click(int &button_cnt, bool &success)
{
    int but = digitalRead(_pin);
    if (but == 0)
    {
        button_cnt++;
        success = false;
    }
}