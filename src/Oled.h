#pragma once
#include <U8g2lib.h>
//extern const u8g2_cb_t u8g2_cb_r0;
//#define U8G2_R0	(&u8g2_cb_r0)
class Oled
{
public:
    Oled(const u8g2_cb_t *, uint8_t, uint8_t);
    void print_ch(int, int, String);
    void print_en(int, int, String);
    void sendbuf();
    void clearbuf();
    void twolines(String, String);

private:
    const u8g2_cb_t *_rot = U8G2_R0;
    uint8_t _Scl, _Sda;
}