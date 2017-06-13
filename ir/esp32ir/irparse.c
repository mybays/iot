#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/rmt.h"
#include "driver/periph_ctrl.h"
#include "soc/rmt_reg.h"

#include "irparse.h"

#define NEC_ITEM_DURATIONS(d)  ((d & 0x7fff)*10/RMT_TICK_10_US)  /*!< Parse duration time from memory register value */


inline bool nec_check_in_range(int duration_ticks, int target_us, float rel_margin)
{
    if(( NEC_ITEM_DURATIONS(duration_ticks) < (target_us*(1+rel_margin)))
        && ( NEC_ITEM_DURATIONS(duration_ticks) > (target_us*(1-rel_margin))))
    {
        return true;
    }
    else
    {
    	//printf("range:%d %d\r\n",NEC_ITEM_DURATIONS(duration_ticks),target_us);
        return false;
    }
}




int check_repeat(rmt_item32_t* item, int item_num)
{
	if(item_num != 2)
	{
		return -1;
	}

	if(item->level0 != RMT_RX_ACTIVE_LEVEL && item->level1 == RMT_RX_ACTIVE_LEVEL)
	{
		printf("level error\r\n");
		return -2;
	}

	if(
		(!nec_check_in_range(item->duration0, NEC_REPEAT_HIGH_US, NEC_BIT_RMARGIN)) ||
		(!nec_check_in_range(item->duration1, NEC_REPEAT_LOW_US, NEC_BIT_RMARGIN))
		)
	{
		printf("1:%d %d\r\n",item->duration0,item->duration1);
		return -3;
	}
	item++;
	if(!nec_check_in_range(item->duration0, NEC_REPEAT_CON_US, NEC_BIT_RMARGIN))
	{
		printf("2:%d %d\r\n",item->duration0,item->duration1);
		return -4;
	}

	return 0;

	/*


	rmt_item32_t* item1= item++;
	rmt_item32_t* item2= item;
	printf("%d:%d   %d:%d\r\n",item1->level0,item1->duration0,item1->level1,item1->duration1);
	printf("%d:%d   %d:%d\r\n",item2->level0,item2->duration0,item2->level1,item2->duration1);
	return 0;
	*/
}

static bool nec_header_if(rmt_item32_t* item)
{
    return true;
    if((item->level0 == RMT_RX_ACTIVE_LEVEL && item->level1 != RMT_RX_ACTIVE_LEVEL)
        && nec_check_in_range(item->duration0, NEC_HEADER_HIGH_US, NEC_BIT_RMARGIN)
        && nec_check_in_range(item->duration1, NEC_HEADER_LOW_US, NEC_BIT_RMARGIN)) {
        return true;
    }
    printf("debug:item->level0 = %d item->duration0 = %d item->level1 = %d item->duration1 = %d\r\n",item->level0,item->duration0,item->level1,item->duration1);

    return false;
}


static bool nec_bit_one_if(rmt_item32_t* item)
{
    if((item->level0 == RMT_RX_ACTIVE_LEVEL && item->level1 != RMT_RX_ACTIVE_LEVEL)
        && nec_check_in_range(item->duration0, NEC_BIT_ONE_HIGH_US, NEC_BIT_RMARGIN)
        && nec_check_in_range(item->duration1, NEC_BIT_ONE_LOW_US, NEC_BIT_RMARGIN)) {
        return true;
    }
    return false;
}

/*
 * @brief Check whether this value represents an NEC data bit 0
 */
static bool nec_bit_zero_if(rmt_item32_t* item)
{
    if((item->level0 == RMT_RX_ACTIVE_LEVEL && item->level1 != RMT_RX_ACTIVE_LEVEL)
        && nec_check_in_range(item->duration0, NEC_BIT_ZERO_HIGH_US, NEC_BIT_RMARGIN)
        && nec_check_in_range(item->duration1, NEC_BIT_ZERO_LOW_US, NEC_BIT_RMARGIN)) {
        return true;
    }
    return false;
}


int getneccode(rmt_item32_t* item, int item_num, uint16_t* addr, uint16_t* data)
{
	int wlen=item_num;
	if(34 != wlen)
	{
		return -1;
	}

	int i = 0, j = 0;
    if(!nec_header_if(item++))
    {
        printf("header check failed\r\n");
        return -1;
    }
    uint16_t addr_t = 0;

    for(j = 0; j < 16; j++)
    {
        if(nec_bit_one_if(item))
        {
            addr_t |= (1 << j);
        }
        else if(nec_bit_zero_if(item))
        {
            addr_t |= (0 << j);
        }
        else
        {
            printf("debug:item->level0 = %d item->duration0 = %d item->level1 = %d item->duration1 = %d\r\n",item->level0,item->duration0,item->level1,item->duration1);
            printf("check addr bit %d failed.\r\n",j);
            return -1;
        }
        item++;
        i++;
    }

    uint16_t data_t = 0;
    for(j = 0; j < 16; j++) {
        if(nec_bit_one_if(item)) {
            data_t |= (1 << j);
        } else if(nec_bit_zero_if(item)) {
            data_t |= (0 << j);
        } else {
            printf("check data bit %d failed.\r\n",j);
            return -1;
        }
        item++;
        i++;
    }
    *addr = addr_t;
    *data = data_t;
    return 0;

}


int media(rmt_item32_t* item, int item_num)
{
	int wlen=item_num;
	if(50 != wlen)
	{
		return -1;
	}

	int i = 0, j = 0;
    if(!nec_header_if(item++))
    {
        printf("header check failed\r\n");
        return -1;
    }
    uint16_t addr_t = 0;

    for(j = 0; j < 16; j++)
    {
        if(nec_bit_one_if(item))
        {
            addr_t |= (1 << j);
        }
        else if(nec_bit_zero_if(item))
        {
            addr_t |= (0 << j);
        }
        else
        {
            printf("debug:item->level0 = %d item->duration0 = %d item->level1 = %d item->duration1 = %d\r\n",item->level0,item->duration0,item->level1,item->duration1);
            printf("check addr bit %d failed.\r\n",j);
            return -1;
        }
        item++;
        i++;
    }

    uint16_t data_t = 0;
    for(j = 0; j < 16; j++) {
        if(nec_bit_one_if(item)) {
            data_t |= (1 << j);
        } else if(nec_bit_zero_if(item)) {
            data_t |= (0 << j);
        } else {
            printf("check data bit %d failed.\r\n",j);
            return -1;
        }
        item++;
        i++;
    }

    uint16_t extra_t = 0;
    for(j = 0; j < 16; j++) {
        if(nec_bit_one_if(item)) {
            extra_t |= (1 << j);
        } else if(nec_bit_zero_if(item)) {
            extra_t |= (0 << j);
        } else {
            printf("check data bit %d failed.\r\n",j);
            return -1;
        }
        item++;
        i++;
    }
    printf("media:0x%04x 0x%04x 0x%04x\r\n",addr_t,data_t,extra_t);
    return 0;

}

int my_nec_parse_items(rmt_item32_t* item, int item_num)
{
	int wlen=item_num;

	if(2 == wlen)
	{
		if(0 == check_repeat(item,item_num))
		{
			printf("repeat\r\n");
		}
		else
		{
			printf("repeat check failed\r\n");
		}
		return 0;
	}

	if(34 == wlen)
	{
		uint16_t rmt_addr;
        uint16_t rmt_cmd;
        if(0 != getneccode(item,item_num,&rmt_addr,&rmt_cmd))
        {
        	return -1;
        }
        printf("addr:0x%04x cmd:0x%04x\r\n",rmt_addr,rmt_cmd);
        return 0;
	}

	if(50 == wlen)
	{
		media(item,item_num);
	}
	return 0;
}