#include "Oled.h"
#include <String.h>
#include <Arduino.h>

Oled::Oled(const u8g2_cb_t *rotation, uint8_t Sda, uint8_t Scl) : _rot(rotation), _Sda(Sda), _Scl(Scl)
{
    U8G2_SSD1306_128X64_NONAME_F_SW_I2C Oled::oled(*rotation, /* clock=*/_Scl, /* data=*/_Sda); //OLED
    Oled::oled.begin();
}

void Oled::print_en(int x, int y, String text)
{
    Oled::oled.setFont(u8g2_font_unifont_t_chinese2);
    Oled::oled.setFontDirection(0);
    Oled::oled.setCursor(x, y);
    Oled::oled.print(text);
    //Oled::oled.sendBuffer();
}

void Oled::print_ch(int x, int y, String text)
{
    Oled::oled.setFont(u8g2_font_unifont_t_chinese1);
    Oled::oled.setFontDirection(0);
    Oled::oled.setCursor(x, y);
    Oled::oled.print(text);
    //Oled::oled.sendBuffer();
}

void Oled::sendbuf()
{
    Oled::oled.sendBuffer();
}

void Oled::clearbuf()
{
    Oled::oled.clearBuffer();
}
void Oled::twolines(String text1, String text2)
{
    Oled::oled.clearbuf();
    Oled::oled.print_ch(0, 15, text1);
    Oled::oled.print_ch(0, 60, text2);
    Oled::oled.sendbuf();
}