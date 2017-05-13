#ifndef _SSD1332_H_
#define _SSD1332_H_
#include <stdint.h>


// Select one of these defines to set the pixel color order
//#define SSD1331_COLORORDER_RGB
#define SSD1331_COLORORDER_BGR

#if defined SSD1331_COLORORDER_RGB && defined SSD1331_COLORORDER_BGR
  #error "RGB and BGR can not both be defined for SSD1331_COLORODER."
#endif

#define WHITE	0xFFFF
#define RED  0x001F
#define GREEN  0x07E0
#define BLUE  0xF800
#define BLACK	0x0000



// SSD1332 Commands

#define SSD1331_CMD_SETCOLUMN 		0x15
#define SSD1331_CMD_SETROW    		0x75

#define SSD1331_CMD_CONTRASTA 		0x81
#define SSD1331_CMD_CONTRASTB 		0x82
#define SSD1331_CMD_CONTRASTC		0x83
#define SSD1331_CMD_MASTERCURRENT 	0x87
#define SSD1331_CMD_SETREMAP 		0xA0
#define SSD1331_CMD_STARTLINE 		0xA1

#define SSD1331_CMD_DISPLAYOFF 		0xAE
#define SSD1331_CMD_DISPLAYON     	0xAF

#define SSD1331_CMD_NOP				0xE3


static const int16_t TFTWIDTH = 96;
static const int16_t TFTHEIGHT = 64;

void ssd1332_data(uint8_t data);
void ssd1332_cmd(uint8_t cmd);

void ssd1332_reset(void);
void ssd1332_init(void);
void ssd1332_clear(uint16_t color);
void ssd1332_putstring(int8_t x,int8_t y,char *str);
void ssd1332_putchar(int8_t x,int8_t y,char c);
void ssd1332_writepix(int8_t x,int8_t y,uint16_t color);

#endif