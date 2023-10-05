/*
 * bsp_sensor.c
 *
 *  Created on: Apr 5, 2022
 *      Author: luckyman
 */

#include "bsp_sensor.h"
#include "global.h"

#include "usart.h"


const uint8_t EnTempCmd[4] = {0x22, 0x00, 0x00, 0x22}; // distance measure command

uint8_t UltraTask_sqc = 0;


uint8_t UltraReturnComplete = 0;
uint8_t UltraReturnIdx = 0 ;
uint8_t UltraReturnBuff[20];  // 20byte는 혹시 모르는 overflow

uint16_t distance;
void UltraTask(void)
{
	switch ( UltraTask_sqc )
	{
		case 0:

			HAL_Delay(50);
			UltraTask_sqc++;
		break;

		case 1:
			if ( UltraReturnComplete != 1 ) break;

			if ( UltraReturnBuff[0] == 0x22 )
			{
				distance = UltraReturnBuff[1] << 8;
				distance = distance + UltraReturnBuff[2];
				UltraTask_sqc = 0;
			}
			else
			{
				UltraTask_sqc = 0;
			}

		break;
	}
}


