#pragma once
#include <U8g2lib.h>
//extern const u8g2_cb_t u8g2_cb_r0;
//#define U8G2_R0	(&u8g2_cb_r0)
class Oled : public U8G2_SSD1306_128X64_NONAME_F_SW_I2C
{
public:
    Oled(const u8g2_cb_t *, uint8_t, uint8_t, uint8_t);
    void print_ch(int, int, String);
    void print_en(int, int, String);
    void twolines(String, String);
    void twolines_en(String, String);

private:
};