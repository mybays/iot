#include <stdlib.h>
#include <stdio.h>

#include <wiringPi.h>

#include "ssd1329.h"



void ssd1329_data(uint8_t data)
{
	digitalWrite(6,HIGH);
	if(-1 == wiringPiSPIDataRW (0,&data, 1))
	{
		printf("error\r\n");
	}
	digitalWrite(6,LOW);
}
void ssd1329_cmd(uint8_t cmd)
{
	digitalWrite(6,LOW);
	if(-1 ==wiringPiSPIDataRW (0,&cmd, 1))
	{
		printf("error\r\n");
	}
}

int main(void)
{
	wiringPiSetup () ;
	pinMode (6, OUTPUT) ;
	if (wiringPiSPISetup (0, 8000000) < 0)
	{
		printf ("SPI Setup failed\n");
		return -1;
	}
	ssd1329_init();
	ssd1329_clear(0xff);
    /*
    ssd1329_putstring(0,0,"HelloWorld!!");
    ssd1329_putstring(0,16,"HelloWorld!!");
    ssd1329_putstring(0,32,"HelloWorld!!");
    ssd1329_putstring(0,48,"HelloWorld!!");
    */
    sleep(1);
    ssd1329_clear(0xFF);

    int k;
    for(k=0;k<96;k++)
    {
        ssd1329_writepix(k,k,0xff);
    }

    ssd1329_putchar(0,0,'H');
    //return;

    ssd1329_putstring(0,0,"HelloWorld!!");
    ssd1329_putstring(0,16,"HelloWorld!!");
    ssd1329_putstring(0,32,"HelloWorld!!");
    ssd1329_putstring(0,48,"HelloWorld!!");
    ssd1329_putstring(0,64,"HelloWorld!!");
    ssd1329_putstring(0,80,"HelloWorld!!");
    return 0;

    int s;
    for(s=0;s<64;s++)
    {
        ssd1329_writepix(s,s,0xff);
        usleep(5*1000);
    }
    for(s=0;s<64;s++)
    {
        ssd1329_writepix(s+1,s,0xff);
        usleep(5*1000);
    }
    for(s=0;s<64;s++)
    {
        ssd1329_writepix(s+2,s,0xff);
        usleep(5*1000);
    }
    

    return 0;
  	
}