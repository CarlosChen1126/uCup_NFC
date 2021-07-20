#include "Oled.h"
#include <String.h>

Oled::Oled(const u8g2_cb_t *rotation, uint8_t Sda, uint8_t Scl) : _rot(rotation), _Sda(Sda), _Scl(Scl)
{
    U8G2_SSD1306_128X64_NONAME_F_SW_I2C oled(*rotation, /* clock=*/_Scl, /* data=*/_Sda); //OLED
}

void Oled::print_en(int x, int y, String text)
{
    oled.setFont(u8g2_font_unifont_t_chinese2);
    oled.setFontDirection(0);
    oled.setCursor(x, y);
    oled.print(text);
    //oled.sendBuffer();
}

void Oled::print_ch(int x, int y, String text)
{
    oled.setFont(u8g2_font_unifont_t_chinese1);
    oled.setFontDirection(0);
    oled.setCursor(x, y);
    oled.print(text);
    //oled.sendBuffer();
}

void Oled::sendbuf()
{
    oled.sendBuffer();
}

void Oled::clearbuf()
{
    oled.clearBuffer();
}
void Oled::twolines(String text1, String text2)
{
    oled.clearbuf();
    oled.print_ch(0, 15, text1);
    oled.print_ch(0, 60, text2);
    oled.sendbuf();
}