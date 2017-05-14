#include "pms5003.h"



/*
void phrase(char *data,int len)
{

    char pm25[12]={0};

    printf("\r\n");

    int value = (data[4]<<8) + data[5];
    printf("PM1.0(CF=1):%d\r\n",value);

    value = (data[6]<<8) + data[7];
    printf("PM2.5(CF=1):%d\r\n",value);

    value = (data[8]<<8) + data[9];
    printf("PM10 (CF=1):%d\r\n",value);

    value = (data[10]<<8) + data[11];
    printf("PM1.0 (STD):%d\r\n",value);

    value = (data[12]<<8) + data[13];
    printf("PM2.5 (STD):%d\r\n",value);
    
    snprintf(pm25,12,"PM2.5:%d",value);
    ssd1329_putstring(0,0,"            ");
    ssd1329_putstring(0,0,pm25);

    value = (data[14]<<8) + data[15];
    printf("PM10 (STD):%d\r\n",value);

    value = (data[16]<<8) + data[17];
    printf(">0.3um     :%d\r\n",value);

    value = (data[18]<<8) + data[19];
    printf(">0.5um     :%d\r\n",value);

    value = (data[20]<<8) + data[21];
    printf(">1.0um     :%d\r\n",value);

    value = (data[22]<<8) + data[23];
    printf(">2.5um     :%d\r\n",value);

    value = (data[24]<<8) + data[25];
    printf(">5.0um     :%d\r\n",value);

    value = (data[26]<<8) + data[27];
    printf(">10um     :%d\r\n",value);

    value = (data[28]<<8) + data[29];
    printf(">jaquan   :%d\r\n",value);
    snprintf(pm25,12,"TVOC:%d",value);
    ssd1329_putstring(0,16,"            ");
    ssd1329_putstring(0,16,pm25);


    if(40 == len)
    {
        value = (data[30]<<8) + data[31];
        printf(">temperature   :%d\r\n",value);

        snprintf(pm25,12,"TEMP:%d.%d C",value/10,value%10);
        ssd1329_putstring(0,32,"            ");
        ssd1329_putstring(0,32,pm25);

        value = (data[32]<<8) + data[33];
        printf(">humidity   :%d\r\n",value);

        snprintf(pm25,12,"HUM:%d.%d%%",value/10,value%10);
        ssd1329_putstring(0,48,"            ");
        ssd1329_putstring(0,48,pm25);
    }
    
}
*/



bool check_sum(char *data,int len)
{
    //保证起始符1=0x42,起始符2=0x4d
    if((data[0] != 0x42) || (data[1] != 0x4d))
    {
        printf("header error:%x %x\r\n",data[0],data[1]);
        return false;
    }

    //保证后面的数据长度是28
    int length = (data[2]<<8) + data[3];
    if((28 != length)&& (36 != length))
    {
        printf("length error:%d\r\n",length);
        return false;
    }

    // 保证checksum检验ok
    int sum=0;
    for(int i=0;i<len-2;i++)
    {
        sum += data[i];
    }
    int check=(data[len-2]<<8)+data[len-1];

    if(sum == check)
    {
        return true;
    }
    printf("checksum error:%x  %x\r\n",sum,check);
    return false;
}


void phrasepm500s(char *data,int len,pms5003_s* pms5003)
{
    pms5003->cfpm1p0 = (data[4]<<8) + data[5];
    pms5003->cfpm2p5 = (data[6]<<8) + data[7];
    pms5003->cfpm10 = (data[8]<<8) + data[9];
    pms5003->stdpm1p0 = (data[10]<<8) + data[11];
    pms5003->stdpm2p5 = (data[12]<<8) + data[13];
    pms5003->stdpm10 = (data[14]<<8) + data[15];
    pms5003->pm0p3count = (data[16]<<8) + data[17];
    pms5003->pm0p5count = (data[18]<<8) + data[19];
    pms5003->pm1p0count = (data[20]<<8) + data[21];
    pms5003->pm2p5count = (data[22]<<8) + data[23];
    pms5003->pm5p0count = (data[24]<<8) + data[25];
    pms5003->pm10count = (data[26]<<8) + data[27];
    pms5003->tvoc = (data[28]<<8) + data[29];
    if(40 == len)
    {
        pms5003->temp = (data[30]<<8) + data[31];
        pms5003->hum = (data[32]<<8) + data[33];
    }
}
