#ifndef LED_H
#define LED_H
class LED
{
public:
    LED(int);
    void blink(int);

private:
    int _pin;
};

#endif
