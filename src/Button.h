#pragma once
// #define holding_threshold 10
class Button
{
public:
    Button(int);
    void click(int &, bool &);
    int getPin();

private:
    int _pin;
    //int holding_tick;
};