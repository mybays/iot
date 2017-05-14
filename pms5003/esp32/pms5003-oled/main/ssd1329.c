#include "ASCII.h"
#include "ssd1329.h"

void ssd1329_reset(void)
{
	ssd1329_init();
}

void ssd1329_init(void)
{
	//usleep(25*1000);
    ssd1329_cmd(0x15);//SET COLUMN ADDR 
    ssd1329_cmd(0x10); 
    ssd1329_cmd(0x3f); 
    ssd1329_cmd(0x75);//SET ROW ADDR 
    ssd1329_cmd(0x00); 
    ssd1329_cmd(0x5f); 

    ssd1329_cmd(0xA0);//SET RE-MAP 
    ssd1329_cmd(0x51); 
    ssd1329_cmd(0xA1);//SET DISPLAY START LINE 
    ssd1329_cmd(0x00); 
    ssd1329_cmd(0xA2);//SET DISPLAY OFFSET 
    ssd1329_cmd(0x60); 
    ssd1329_cmd(0xA4);//SET DISPLAY MODE 
    ssd1329_cmd(0xA8);//SET MUX RADIO 
    ssd1329_cmd(0x5F); 
    ssd1329_cmd(0xB2);//SET FRAME FREQUENCY 
    ssd1329_cmd(0x23); 
    ssd1329_cmd(0xB3);//SET FRONT CLOCK DIVIDER & OSCILLATOR FREQUENCY 
    ssd1329_cmd(0xF0); 

    ssd1329_cmd(0x81);//SET CONTRAST CURRENT 
    ssd1329_cmd(0x3F);// 
    ssd1329_cmd(0xBC);    //first pre_charge voltage 
    ssd1329_cmd(0x1F); 
    //vTaskDelay(10 / portTICK_RATE_MS);
    //usleep(10*1000);
    ssd1329_cmd(0x82);   //second pre_charge speed 
    ssd1329_cmd(0xFE); 
    //vTaskDelay(10 / portTICK_RATE_MS);
    //usleep(10*1000);
    ssd1329_cmd(0xB1);   //first pre_charge phase length 
    ssd1329_cmd(0x21); 

    ssd1329_cmd(0xBB);//SET SECONDE PRE_CHARGE PERIOD 
    ssd1329_cmd(0x0F); 

    ssd1329_cmd(0xbe);//SET VCOMH 
    ssd1329_cmd(0x1F); 

    ssd1329_cmd(0xB8);        //SET GS 
    ssd1329_cmd(0x04); //GS1 
    ssd1329_cmd(0x06); //GS2 
    ssd1329_cmd(0x08); //GS3         
    ssd1329_cmd(0x0A); //GS4 
    ssd1329_cmd(0x0C); //GS5  
    ssd1329_cmd(0x0E); //GS6       
    ssd1329_cmd(0x10); //GS7 
    ssd1329_cmd(0x12); //GS8 
    ssd1329_cmd(0x14); //GS9   
    ssd1329_cmd(0x16); //GS10 
    ssd1329_cmd(0x18); //GS11 
    ssd1329_cmd(0x1A); //GS12 
    ssd1329_cmd(0x1C); //GS13 
    ssd1329_cmd(0x1E); //GS14 
    ssd1329_cmd(0x20); //GS15 
    //vTaskDelay(10 / portTICK_RATE_MS);
    //usleep(10*1000);
         
    ssd1329_cmd(0xAF);         //DSPLAY ON

    ssd1329_clear(0x00);
}

void ssd1329_clear(uint16_t depth)
{
	unsigned short x,y;   
    ssd1329_cmd(0x15);//SET COLUMN ADDR 
    ssd1329_cmd(0x10); 
    ssd1329_cmd(63); 
    ssd1329_cmd(0x75);//SET ROW ADDR 
    ssd1329_cmd(0x00); 
    ssd1329_cmd(95);  

    for(x=0;x<96;x++)
        for(y=0;y<48;y++)
    {
        ssd1329_data(depth);//显示所填充的颜色. 
    }
}

void ssd1329_writepix(int8_t x,int8_t y,uint8_t depth)
{
	ssd1329_cmd(0x15);//SET COLUMN ADDR 
    ssd1329_cmd(0x10+(x/2)); 
    ssd1329_cmd(0x10+(x/2)); 
    ssd1329_cmd(0x75);//SET ROW ADDR 
    ssd1329_cmd(0x00+y); 
    ssd1329_cmd(0x00+y);

    if(x&0x01)
    {
        ssd1329_data(0x0f);
    }
    else
    {
        ssd1329_data(0xf0);
    }
}

void ssd1329_putchar(int8_t x,int8_t y,char c)
{
	unsigned char temp;
	unsigned int pos,t,i;  
	unsigned char size; 
	size=16;	//找到字体大小
	ssd1329_cmd(0x15);//SET COLUMN ADDR 
	ssd1329_cmd(0x10+(x/2)); 
	ssd1329_cmd(0x10+(x/2)+3); 
	ssd1329_cmd(0x75);//SET ROW ADDR 
	ssd1329_cmd(0x00+y); 
	ssd1329_cmd(0x00+y+15);  
	c=c-' ';//得到偏移后的值
	i=c*16;

	/*
	for(pos=0;pos<size;pos++)
	{
		for(t=0;t<4;t++)
		{
			ssd1329_data(0x00);
		}
	}
	return;
	*/

	for(pos=0;pos<size;pos++)
	{
		temp=nAsciiDot[i+pos];	//调通调用艺术字体
		//printf("0x%x\r\n",temp);
		unsigned char sss;
		for(t=0;t<4;t++)
		{                 
			if(temp&0x80)
			{
				sss=0xf0;
			}
			else
			{
				sss=0x00;
			}
			temp<<=1;
			if(temp&0x80)
			{
				sss+=0x0f;
			}
			else
			{
				sss+=0x00;
			}
			ssd1329_data(sss);
			temp<<=1;
			//printf("%x ",sss);
		}
		//printf("\r\n");
	}
}

void ssd1329_putstring(int8_t x,int8_t y,char *str)
{
	unsigned char l=0;
	while(*str) 
	{
		if( *str < 0x80) 
		{
			ssd1329_putchar(x+l,y,*str);
			str++;l+=8;
		}
		else
		{
			//PutGB1616(x+l,y,(unsigned char*)s);
			//s+=2;l=l+8;
		}
	}
}