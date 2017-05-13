#include "ASCII.h"
#include "ssd1332.h"


void ssd1332_reset(void)
{
	ssd1332_init();
}

void ssd1332_init(void)
{
	ssd1332_cmd(SSD1331_CMD_DISPLAYOFF); // 关闭显示

	ssd1332_cmd(SSD1331_CMD_CONTRASTA); //设置A类颜色对比度
	ssd1332_cmd(0xfe);
	ssd1332_cmd(SSD1331_CMD_CONTRASTB); //设置B类颜色对比度
	ssd1332_cmd(0xfe);
	ssd1332_cmd(SSD1331_CMD_CONTRASTC); //设置C类颜色对比度
	ssd1332_cmd(0xfe);
	ssd1332_cmd(SSD1331_CMD_MASTERCURRENT); //设置驱动电流强度
	ssd1332_cmd(0x0f);
	ssd1332_cmd(SSD1331_CMD_SETREMAP); //设置为65k颜色模式
/*
#if defined SSD1331_COLORORDER_RGB
    ssd1332_cmd(0x72);				// RGB Color
#else
    ssd1332_cmd(0x76);				// BGR Color
#endif
*/
	ssd1332_cmd(0x66);

	ssd1332_cmd(SSD1331_CMD_STARTLINE); 	// 0xA1
    ssd1332_cmd(0x0);


	ssd1332_cmd(SSD1331_CMD_DISPLAYON); // 打开显示

	return;
}

void ssd1332_clear(uint16_t color)
{
	ssd1332_cmd(SSD1331_CMD_SETCOLUMN);
	ssd1332_cmd(0x00);
	ssd1332_cmd(TFTWIDTH-1);
	ssd1332_cmd(SSD1331_CMD_SETROW);
	ssd1332_cmd(0x00);
	ssd1332_cmd(TFTHEIGHT-1);

	uint16_t i,x,y;
	//for(i=0;i<TFTWIDTH*TFTHEIGHT;i++)
	for(x=0;x<96;x++)
	for(y=0;y<64;y++)
	{
		ssd1332_data(color>>8);//显示所填充的颜色. 
        ssd1332_data(color);//显示所填充的颜色.
	}
	ssd1332_cmd(SSD1331_CMD_NOP);

}

void ssd1332_goto(int8_t x,int8_t y)
{
	ssd1332_cmd(SSD1331_CMD_SETCOLUMN);
	ssd1332_cmd(x);
	ssd1332_cmd(TFTHEIGHT-1);
	ssd1332_cmd(SSD1331_CMD_SETROW);
	ssd1332_cmd(y);
	ssd1332_cmd(TFTWIDTH-1);
}

void ssd1332_putstring(int8_t x,int8_t y,char *str)
{
	//ssd1332_goto(x,y);
	uint8_t l=0;
	while(*str)
	{
		if( *str < 0x80)
		{
			ssd1332_putchar(x+l,y,*str);
			str++;
			l+=8;
		}
		else
		{
			//ssd1332_putchar(x+l,y,*str);
			str+=2;
			l+=16;
		}
	}
}

void ssd1332_putchar(int8_t x,int8_t y,char c)
{
	char temp;
	int16_t pos,t,i;
	int8_t size=16;

	ssd1332_cmd(SSD1331_CMD_SETCOLUMN);
	ssd1332_cmd(x);
	ssd1332_cmd(x+7);
	ssd1332_cmd(SSD1331_CMD_SETROW);
	ssd1332_cmd(y);
	ssd1332_cmd(y+16);

	c=c-' ';
	i=c*16;

	for(pos=0;pos<size;pos++)
	{
		temp=nAsciiDot[i+pos];
		for(t=0;t<8;t++)
		{
			if(temp&0x80)
			{
				ssd1332_data(GREEN>>8);
				ssd1332_data((uint8_t)(GREEN|0x0f));
			}
			else
			{
				ssd1332_data(0);
				ssd1332_data(0);
			}
			temp<<=1;
		}
	}
}

void ssd1332_writepix(int8_t x,int8_t y,uint16_t color)
{
	ssd1332_cmd(SSD1331_CMD_SETCOLUMN);//SET COLUMN ADDR 
	ssd1332_cmd(x); 
	ssd1332_cmd(x); 
	ssd1332_cmd(SSD1331_CMD_SETROW);//SET ROW ADDR 
	ssd1332_cmd(y); 
	ssd1332_cmd(y);

	ssd1332_data(color>>8);//显示所填充的颜色. 
	ssd1332_data(color);//显示所填充的颜色.
}