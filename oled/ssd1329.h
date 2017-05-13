#ifndef _SSD1329_H_
#define _SSD1329_H_
#include <stdint.h>

static const int16_t TFTWIDTH = 96;
static const int16_t TFTHEIGHT = 64;

void ssd1329_data(uint8_t data);
void ssd1329_cmd(uint8_t cmd);

void ssd1329_reset(void);
void ssd1329_init(void);
void ssd1329_clear(uint16_t depth);
void ssd1329_putstring(int8_t x,int8_t y,char *str);
void ssd1329_putchar(int8_t x,int8_t y,char c);
void ssd1329_writepix(int8_t x,int8_t y,uint8_t depth);

#endif