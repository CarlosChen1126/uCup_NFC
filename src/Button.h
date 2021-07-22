#pragma once
class Button
{
public:
    Button(int);
    void click(int &, bool &);
    int getPin();

private:
    int _pin;
};