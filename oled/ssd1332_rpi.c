#include <stdlib.h>
#include <stdio.h>

#include <wiringPi.h>

#include "ssd1332.h"



void ssd1332_data(uint8_t data)
{
	digitalWrite(6,HIGH);
	if(-1 == wiringPiSPIDataRW (0,&data, 1))
	{
		printf("error\r\n");
	}
	digitalWrite(6,LOW);
}
void ssd1332_cmd(uint8_t cmd)
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
	ssd1332_init();
	ssd1332_clear(RED);
    //sleep(1);
    ssd1332_clear(GREEN);
    //sleep(1);
    ssd1332_clear(BLUE);
    //sleep(1);
    ssd1332_clear(BLACK);
    //sleep(1);
    ssd1332_putstring(0,0,"HelloWorld!!");
    ssd1332_putstring(0,16,"HelloWorld!!");
    ssd1332_putstring(0,32,"HelloWorld!!");
    ssd1332_putstring(0,48,"HelloWorld!!");

    sleep(1);
    ssd1332_clear(BLACK);
    int s;
    for(s=0;s<64;s++)
    {
        ssd1332_writepix(s,s,RED);
        usleep(5*1000);
    }
    for(s=0;s<64;s++)
    {
        ssd1332_writepix(s+1,s,GREEN);
        usleep(5*1000);
    }
    for(s=0;s<64;s++)
    {
        ssd1332_writepix(s+2,s,BLUE);
        usleep(5*1000);
    }
    

    return 0;
  	
}