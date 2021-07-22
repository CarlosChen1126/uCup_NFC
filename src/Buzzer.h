#ifndef BUZZER_H
#define BUZZER_H
class Buzzer
{
public:
    Buzzer(int);
    void buzz(int);
    int getPin();

private:
    int _pin;
};

#endif