/*
 * global.c
 *
 *  Created on: 2022. 3. 24.
 *      Author: luckyman
 */


#include "global.h"
#include "sensor_drv.h"
#define ON 1
#define OFF 0
uint8_t sen_id;


/*
#define NTC_SEN_ID                0x00
#define PHASE_2_CUR_ID            0x08  
#define PHASE_3_CUR_ID            0x07
#define PHASE_2_VOL_ID            0x02
#define PHASE_3_VOL_ID            0x01
#define WATER_LEAK_ID             0x04
#define UL_SONIC_ID               0x06
#define PREASURE_ID               0x03
#define VIB_ID                    0x05

*/


void power_init()
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);  // 5v volt enable
	__NOP();__NOP();__NOP();
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);  // 3v volt enable
}

#define SENSOR_ID_MASK  0x000F
void read_sen_id()
{
	uint16_t sensorChercker = GPIOB->IDR & SENSOR_ID_MASK;
	sen_id = (uint8_t)sensorChercker; 
	sen_id =1;
}


const char hextable[] = {
 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,-1,-1,-1,-1,-1,-1,
-1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
-1,10,11,12,13,14,15,
};

const char chtable[] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};
long ch2hex(char *hex, uint8_t size) {
   long ret = 0;

   while(size > 0)
   {
	   if(*hex < 48 || *hex > 102)
		   return -0x100;

	   if(hextable[*hex - 48] == -1)
		   return -0x100;

	   ret = (ret << 4) | hextable[*hex - 48];

	   size--;
	   hex++;
   }
   return ret;
}

void hex2ch(uint8_t hex, char *chBuff)
{
	for(int i = 0; i < 2; i++)
	{
		chBuff[i] = chtable[(hex >> (4 - i*4)) & 0x0F];
	}
	return;
}
