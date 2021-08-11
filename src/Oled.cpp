#include "Oled.h"
//#include <String.h>
#include <Arduino.h>

//constructor inherent from U8G2_SSD1306_128X64_NONAME_F_SW_I2C
Oled::Oled(const u8g2_cb_t *rotation, uint8_t Scl, uint8_t Sda, uint8_t Rset) : U8G2_SSD1306_128X64_NONAME_F_SW_I2C(rotation, Scl, Sda, Rset)
{
    begin();
    enableUTF8Print();
    clearBuffer();
}
//print one english sentence
void Oled::print_en(int x, int y, String text)
{
    setFont(u8g2_font_unifont_t_chinese2);
    setFontDirection(0);
    setCursor(x, y);
    print(text);
    //sendBuffer();
}
//print one chinese sentence
void Oled::print_ch(int x, int y, String text)
{
    setFont(u8g2_font_unifont_t_chinese1);
    setFontDirection(0);
    setCursor(x, y);
    print(text);
    //sendBuffer();
}
//print two chinese sentences
void Oled::twolines(String text1, String text2)
{
    clearBuffer();
    print_ch(0, 15, text1);
    print_ch(0, 60, text2);
    sendBuffer();
}
//print two english sentences
void Oled::twolines_en(String text1, String text2)
{
    clearBuffer();
    print_en(0, 15, text1);
    print_en(0, 60, text2);
    sendBuffer();
}

void Oled::printc(int x, int y, String text)
{
    clearBuffer();
    setFont(u8g2_font_unifont_t_chinese1);
    setFontDirection(0);
    setCursor(x, y);
    print(text);
    sendBuffer();
}
void Oled::printe(int x, int y, String text)
{
    clearBuffer();
    setFont(u8g2_font_unifont_t_chinese2);
    setFontDirection(0);
    setCursor(x, y);
    print(text);
    sendBuffer();
}