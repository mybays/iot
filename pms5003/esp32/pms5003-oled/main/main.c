/* Uart Events Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "driver/uart.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "soc/uart_struct.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"


#include "ssd1329.h"

/**
 * This is a example exaple which echos any data it receives on UART1 back to the sender, with hardware flow control
 * turned on. It does not use UART driver event queue.
 *
 * - port: UART1
 * - rx buffer: on
 * - tx buffer: off
 * - flow control: on
 * - event queue: off
 * - pin assignment: txd(io4), rxd(io5), rts(18), cts(19)
 */

#define ECHO_TEST_TXD  (4)
#define ECHO_TEST_RXD  (5)
#define ECHO_TEST_RTS  (18)
#define ECHO_TEST_CTS  (19)

#define PIN_NUM_MISO 25
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  19
#define PIN_NUM_CS   22

#define PIN_NUM_DC   21
#define PIN_NUM_RST  18
#define PIN_NUM_BCKL 5


#define BUF_SIZE (1024)

spi_device_handle_t spi;


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

void phrase(char *data,int len)
{
    /*
    uint16_t *pvalue;
    for(int i=0;i<16;i++)
    {
        pvalue=(uint16_t *)data+i;
        printf("%x ",*pvalue);
    }
    */
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
    
    snprintf(pm25,6,"%4d",value);
    //ssd1329_putstring(48,0,"      ");
    ssd1329_putstring(48,0,pm25);

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
    snprintf(pm25,7,"%4d",value);
    //ssd1329_putstring(40,16,"       ");
    ssd1329_putstring(40,16,pm25);


    if(40 == len)
    {
        value = (data[30]<<8) + data[31];
        printf(">temperature   :%d\r\n",value);

        snprintf(pm25,7,"%2d.%1d C",value/10,value%10);
        //ssd1329_putstring(40,32,"       ");
        ssd1329_putstring(40,32,pm25);

        value = (data[32]<<8) + data[33];
        printf(">humidity   :%d\r\n",value);

        snprintf(pm25,8,"%3d.%1d%%",value/10,value%10);
        //ssd1329_putstring(32,48,"        ");
        ssd1329_putstring(32,48,pm25);
    }
    
}


//Send a command to the ILI9341. Uses spi_device_transmit, which waits until the transfer is complete.
void ili_cmd(spi_device_handle_t spi, const uint8_t cmd) 
{
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=8;                     //Command is 8 bits
    t.tx_buffer=&cmd;               //The data is the cmd itself
    t.user=(void*)0;                //D/C needs to be set to 0
    ret=spi_device_transmit(spi, &t);  //Transmit!
    assert(ret==ESP_OK);            //Should have had no issues.
}

//Send data to the ILI9341. Uses spi_device_transmit, which waits until the transfer is complete.
void ili_data(spi_device_handle_t spi, const uint8_t *data, int len) 
{
    esp_err_t ret;
    spi_transaction_t t;
    if (len==0) return;             //no need to send anything
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=len*8;                 //Len is in bytes, transaction length is in bits.
    t.tx_buffer=data;               //Data
    t.user=(void*)1;                //D/C needs to be set to 1
    ret=spi_device_transmit(spi, &t);  //Transmit!
    assert(ret==ESP_OK);            //Should have had no issues.
}


void ssd1329_data(uint8_t data)
{
    ili_data(spi,&data,1);
}
void ssd1329_cmd(uint8_t cmd)
{
    ili_cmd(spi,cmd);
}

//This function is called (in irq context!) just before a transmission starts. It will
//set the D/C line to the value indicated in the user field.
void ili_spi_pre_transfer_callback(spi_transaction_t *t) 
{
    int dc=(int)t->user;
    gpio_set_level(PIN_NUM_DC, dc);
}

//an example of echo test with hardware flow control on UART1
static void echo_task()
{
    const int uart_num = UART_NUM_1;
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
    };
    //Configure UART1 parameters
    uart_param_config(uart_num, &uart_config);
    //Set UART1 pins(TX: IO4, RX: I05, RTS: IO18, CTS: IO19)
    uart_set_pin(uart_num, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS);
    //Install UART driver (we don't need an event queue here)
    //In this example we don't even use a buffer for sending data.
    uart_driver_install(uart_num, BUF_SIZE * 2, 0, 0, NULL, 0);


    esp_err_t ret;
    spi_bus_config_t buscfg={
        .miso_io_num=PIN_NUM_MISO,
        .mosi_io_num=PIN_NUM_MOSI,
        .sclk_io_num=PIN_NUM_CLK,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1
    };
    spi_device_interface_config_t devcfg={
        .clock_speed_hz=20000000,               //Clock out at 10 MHz
        .mode=0,                                //SPI mode 0
        .spics_io_num=PIN_NUM_CS,               //CS pin
        .queue_size=7,                          //We want to be able to queue 7 transactions at a time
        .pre_cb=ili_spi_pre_transfer_callback,  //Specify pre-transfer callback to handle D/C line
    };
    //Initialize the SPI bus
    ret=spi_bus_initialize(HSPI_HOST, &buscfg, 1);
    assert(ret==ESP_OK);
    //Attach the LCD to the SPI bus
    ret=spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
    assert(ret==ESP_OK);
    //Initialize the LCD
    gpio_set_direction(PIN_NUM_DC, GPIO_MODE_OUTPUT);

    ssd1329_init();

    ssd1329_clear(0xff);
    vTaskDelay(1000 / portTICK_RATE_MS);
    ssd1329_clear(0x00);

    uint8_t* data = (uint8_t*) malloc(BUF_SIZE);

    //预先写前缀
    ssd1329_putstring(0,0,"PM2.5:");
    ssd1329_putstring(0,16,"TVOC:");
    ssd1329_putstring(0,32,"TEMP:");
    ssd1329_putstring(0,48,"HUM:");
    

    while(1)
    {
        //Read data from UART
        int len = uart_read_bytes(uart_num, data, BUF_SIZE, 20 / portTICK_RATE_MS);
        //Write data back to UART
        if(0 != len)
        {
            printf("len=%d\r\n",len);
            if((32 == len)||(40 == len))
            {
                bool isok = check_sum((char*)data,len);
                if(isok)
                {
                    phrase((char*)data,len);
                }
                else
                {
                    printf("data error\r\n");
                }
            }
        }
    }
}

void app_main()
{
    //A uart read/write example without event queue;
    xTaskCreate(echo_task, "uart_echo_task", 1024, NULL, 10, NULL);
}
