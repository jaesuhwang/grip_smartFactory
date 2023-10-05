/*
 * packet.c
 *
 *  Created on: 2022. 1. 31.
 *      Author: lucky
 */

#include "packet.h"
#include "usart.h"
#include "backup.h"
#include "water_dec.h"
#define RX_BUFFERSIZE 32

uint8_t uart_flag;
uint8_t conf_flag = 1;

uint32_t used;

ringbuffer_t rxrb;
uint8_t rxdata[RX_BUFFERSIZE];
uint8_t cmdBuffer[RX_BUFFERSIZE];

#define UART_TIMEOUT 50 //milliseconds

static uint16_t pktLen = 0;
config_data configs;
#define DATA_EEPROM_BASE_ADDR 0x08030000
#define DATA_EEPROM_BASE_ADDR2 0x08030010

enum
{
	GROUP,
	NODE,
	CHANNEL,
	TX_POWER,
	TX_TIME,
	RF_CALIBRATION,
	SLEEP_ON,
	DBG,
	SENSOR_OFFSET,
	SERIAL_NUMBER,
	
};


typedef struct
{
	uint8_t pos;
	uint8_t size;
}FLASH_DATA_POS_TAB_t;

const FLASH_DATA_POS_TAB_t FlashDataPosTab[] =
{
		{0, 1},
		{1, 1},
		{2, 1},
		{3, 1},
		{4, 1},
		{5, 2},
		{7, 1},
		{8, 1},
		{9, 6},
		{16, 8}
		
};

void uartRingBuff_init()
{
	ringbuf_init(&rxrb, rxdata, RX_BUFFERSIZE);
}


int recvUartPkt()
{
	uint8_t c, i = 0, l = 0;
	//uint32_t used;

	for(i = 0; i < RX_BUFFERSIZE; i++)
		cmdBuffer[i] = 0;

	used = ringbuf_used(&rxrb);

	for (i = 0; i < used; i++)
	{
		c = ringbuf_readone(&rxrb);
		if(c == '\r' || c == '\n')
		{
		  pktLen = i;
		  return pktLen;
		}
		else
		  cmdBuffer[i] = c;
	}
	pktLen = 0;
	return -1;
}



#define WORD_BYTE		4

uint8_t* ReadFlash2Buff(uint16_t idx ,uint8_t* data , uint8_t size, uint8_t Base_addr)
{
	uint32_t FlashReadData[512] = {0x0,};
	if ( Base_addr == 0 )
	{
		Read_ST_Flash(DATA_EEPROM_BASE_ADDR , FlashReadData , 512);
	}
	else if( Base_addr == 1)
	{
		Read_ST_Flash(DATA_EEPROM_BASE_ADDR2 , FlashReadData , 512);
	}
	uint16_t array_idx = idx >> 2;
	int array_pos = 3- (idx % WORD_BYTE);

	uint8_t *p_FlashReadData = &FlashReadData[array_idx];

	for ( int i = 0 ; i<size; i++)
	{
		data[i] = p_FlashReadData[array_pos--];
		if ( array_pos < 0)
		{
			array_idx++;
			p_FlashReadData = &FlashReadData[array_idx];
			array_pos = 3;
		}

	}

}



void CopyBuff2FlashWrite(uint16_t idx , uint8_t* data, uint8_t size)
{

	uint32_t FlashReadData[512] = {0x0,};
	Read_ST_Flash(DATA_EEPROM_BASE_ADDR , FlashReadData , 512);

	//uint16_t array_idx = idx / WORD_BYTE ;
	uint16_t array_idx = idx >> 2;
	int array_pos = 3- (idx % WORD_BYTE);

	uint8_t *p_FlashReadData = &FlashReadData[array_idx];

	for ( int i = 0 ; i<size; i++)
	{
		p_FlashReadData[array_pos--] = data[i];
		if ( array_pos < 0)
		{
			array_idx++;
			p_FlashReadData = &FlashReadData[array_idx];
			array_pos = 3;
		}

	}
	Write_ST_Flash(DATA_EEPROM_BASE_ADDR, &FlashReadData , 256);
}



int read_configuration_data(config_data *c)
{
	uint32_t d0 = *(uint32_t *) (DATA_EEPROM_BASE_ADDR);
	if ( d0 != 0xFFFFFFFF) // �����Ͱ� �ֵ�
	{
		ReadFlash2Buff( FlashDataPosTab[GROUP].pos, &configs.sensorID, FlashDataPosTab[GROUP].size,0 );
		ReadFlash2Buff( FlashDataPosTab[NODE].pos, &configs.NID, FlashDataPosTab[NODE].size,0 );
		ReadFlash2Buff( FlashDataPosTab[CHANNEL].pos, &configs.ch, FlashDataPosTab[CHANNEL].size,0 );
		ReadFlash2Buff( FlashDataPosTab[TX_POWER].pos, &configs.pw, FlashDataPosTab[TX_POWER].size,0 );
		ReadFlash2Buff( FlashDataPosTab[TX_TIME].pos, &configs.txP, FlashDataPosTab[TX_TIME].size,0 );
		ReadFlash2Buff( FlashDataPosTab[RF_CALIBRATION].pos, &configs.fCal, FlashDataPosTab[RF_CALIBRATION].size,0 );
		ReadFlash2Buff( FlashDataPosTab[SLEEP_ON].pos, &configs.sleepOn, FlashDataPosTab[SLEEP_ON].size,0 );
		ReadFlash2Buff( FlashDataPosTab[DBG].pos, &configs.DBG_led, FlashDataPosTab[DBG].size,0 );
		ReadFlash2Buff( FlashDataPosTab[SERIAL_NUMBER].pos, &configs.serialNumber, FlashDataPosTab[SERIAL_NUMBER].size,1);
		return 0;
	}
	else // ������ �ʱⰪ
	{
		configs.sensorID = 0x01;
		configs.NID = 0x01;
		configs.ch = 0;
		configs.pw = 18;
		configs.txP = 3;
		configs.fCal = 0x0006;
		configs.sleepOn = 0;
		configs.DBG_led = 1;
		configs.sensorID = 1;

//		configs.serialNumber = 0x0123456789ABCDEF;


		CopyBuff2FlashWrite( FlashDataPosTab[GROUP].pos, &configs.sensorID, FlashDataPosTab[GROUP].size);
		CopyBuff2FlashWrite( FlashDataPosTab[NODE].pos, &configs.NID, FlashDataPosTab[NODE].size);
		CopyBuff2FlashWrite( FlashDataPosTab[CHANNEL].pos, &configs.ch, FlashDataPosTab[CHANNEL].size);
		CopyBuff2FlashWrite( FlashDataPosTab[TX_POWER].pos, &configs.pw, FlashDataPosTab[TX_POWER].size);
		CopyBuff2FlashWrite( FlashDataPosTab[TX_TIME].pos, &configs.txP, FlashDataPosTab[TX_TIME].size);
		CopyBuff2FlashWrite( FlashDataPosTab[RF_CALIBRATION].pos, &configs.fCal, FlashDataPosTab[RF_CALIBRATION].size);
		CopyBuff2FlashWrite( FlashDataPosTab[SLEEP_ON].pos, &configs.sleepOn, FlashDataPosTab[SLEEP_ON].size);
		CopyBuff2FlashWrite( FlashDataPosTab[DBG].pos, &configs.sleepOn, FlashDataPosTab[DBG].size);
		CopyBuff2FlashWrite( FlashDataPosTab[SERIAL_NUMBER].pos, &configs.serialNumber, FlashDataPosTab[SERIAL_NUMBER].size);
		return -1;
	}

	
}


/*
1. Set Group ID (HEX, 1byte)
IN:  SG00 ~ SGFF
OUT: OK | ERR

   Get Group ID
IN:  GG
OUT: GG00 ~ GGFF | ERR

2. Set Node ID (HEX, 1byte)
IN:  SN00 ~ SNFF
OUT: OK | ERR

   Get Node ID
IN:  GN
OUT: GN00 ~ GNFF | ERR

3. Set SERIAL NUMBER (HEX, 8byte)
      <0123456789ABCDEF>   <0123456789ABCDEF>
IN:  SS 0000000000000000 ~ SSFFFFFFFFFFFFFFFF
OUT: OK | ERR

   Get SERIAL NUMBER
IN:  GS
OUT:  <0123456789ABCDEF>   <0123456789ABCDEF>
     GS0000000000000000 ~ GSFFFFFFFFFFFFFFFF | ERR

4. Set Channel (HEX, 1byte)
IN:  SC00 ~ SC10
OUT: OK | ERR

   Get Channel
IN:  GC
OUT: GC00 ~ GC10 | ERR

5. Set TX power (HEX, 1byte, 0 ~ 255)
IN:  SP00 ~ SPFF
OUT: OK | ERR

   Get TX Power
IN:  GP
OUT: GP00 ~ GPFF | ERR

6. Set TX TIME (HEX, 1byte, 0 ~ 255 hours)
IN:  ST00 ~ STFF
OUT: OK | ERR

   Get TX TIME
IN:  GT
OUT: GT00 ~ GTFF | ERR

7. Set RF CALIBRATION VALUE (HEX, 1byte,  Frequency cal. value( * 10 Hz) -327.68 KHz ~ 327.67 KHz
IN:  SR0000 ~ SRFFFF
OUT: OK | ERR

   Get RF CALIBRATION VALUE
IN:  GR
OUT: GR0000 ~ GRFFFF | ERR

8. Continuous TX (HEX, 1byte, if you want to adjust frequency, use the "Set RF CALBRATION VALUE".)
IN:  CT00 ~ CT10
OUT: OK | ERR

9. RESET (return to normal operation)
IN: RS
OUT: OK | ERR

10. Set SENSOR DATA TABLE ( Temperature, Smoke? )
IN: SD00xxxx
OUT: OK | ERR

    Get SENSOR DATA TABLE
IN: GD
OUT: (Temperature, 110 * 2 bytes)
     00xxxx\r\n
     02xxxx\r\n
     ...
     6Exxxx\r\n

*/
uint8_t sNumber[16];


int handleSetFunc()
{
	short rShort,test1,test2;
	uint8_t rU8;
	//char rC8;


	uint32_t TempMemoryStrobe[2] = {0x0,0x0};
	uint32_t TempStrobe;
	uint32_t BuffFlashStrobe[4] = {0,};
	int ret = 0;

	switch(cmdBuffer[1])
	{
		case 'G':  // Group
			rShort = (short)ch2hex((char*)&cmdBuffer[2], 2);
			if(rShort != -0x100)
			{
				rU8 = (uint8_t) rShort;
				CopyBuff2FlashWrite( FlashDataPosTab[GROUP].pos, &rU8, FlashDataPosTab[GROUP].size);
			}
			else
				ret = -1;
			break;

		case 'L':  // DBG LED
			rShort = (short)ch2hex((char*)&cmdBuffer[2], 2);
			if(rShort != -0x100)
			{
				rU8 = (uint8_t) rShort;
				CopyBuff2FlashWrite( FlashDataPosTab[DBG].pos, &rU8, FlashDataPosTab[DBG].size);
			}
			else
				ret = -1;
			break;

		case 'N':  // Node  
			rShort = (short)ch2hex((char*)&cmdBuffer[2], 2);
			if(rShort != -100)
			{
				rU8 = (uint8_t) rShort;
				CopyBuff2FlashWrite( FlashDataPosTab[NODE].pos, &rU8, FlashDataPosTab[NODE].size);
			}
			else
				ret = -1;
			break;

		case 'C':	// Channel
			rShort = (short)ch2hex((char*)&cmdBuffer[2], 2);
			if(rShort != -100)
			{
				rU8 = (uint8_t) rShort;
				CopyBuff2FlashWrite( FlashDataPosTab[CHANNEL].pos, &rU8, FlashDataPosTab[CHANNEL].size);
			}
			else
				ret = -1;
			break;

		case 'P':	// Txpower
			rShort = (short)ch2hex((char*)&cmdBuffer[2], 2);
			if(rShort != -100)
			{
				rU8 = (uint8_t) rShort;
				CopyBuff2FlashWrite( FlashDataPosTab[TX_POWER].pos, &rU8, FlashDataPosTab[TX_POWER].size);
			}
			else
				ret = -1;
			break;

		case 'T':
			rShort = (short)ch2hex((char*)&cmdBuffer[2], 2);
			if(rShort != -100)
			{
				rU8 = (uint8_t) rShort;
				CopyBuff2FlashWrite( FlashDataPosTab[TX_TIME].pos, &rU8, FlashDataPosTab[TX_TIME].size);
			}
			else
				ret = -1;
			break;

		case 'R':
			for(rU8 = 0; rU8 < 4; rU8 += 2)
			{
				rShort = (short)ch2hex((char*)&cmdBuffer[2 + rU8], 2);
				
				if(rShort != -100)
					sNumber[rU8 / 2] = (uint8_t)rShort;
				else
				{
					ret = -1;
					break;
				}
			}
			CopyBuff2FlashWrite( FlashDataPosTab[RF_CALIBRATION].pos, &sNumber, FlashDataPosTab[RF_CALIBRATION].size);
			break;

		case 'J':  // SJ sleep on off 
			rShort = (short)ch2hex((char*)&cmdBuffer[2], 2);
			if(rShort != -100)
			{
				rU8 = (uint8_t) rShort;
				CopyBuff2FlashWrite( FlashDataPosTab[SLEEP_ON].pos, &rU8, FlashDataPosTab[SLEEP_ON].size);
			}
			else
				ret = -1;
			break;

		case 'S': // SERIAL NUMBER
			for(rU8 = 0; rU8 < 16; rU8 += 2)
			{
				rShort = (short)ch2hex((char*)&cmdBuffer[2 + rU8], 2);
				if(rShort != -100)
					sNumber[rU8 / 2] = (uint8_t)rShort;
				else
				{
					ret = -1;
					break;
				}
			}
			CopyBuff2FlashWrite( FlashDataPosTab[SERIAL_NUMBER].pos, &sNumber, FlashDataPosTab[SERIAL_NUMBER].size);
			break;

		case 'D':
			for(rU8 = 0; rU8 < 8; rU8 += 2)
			{
				rShort = (short)ch2hex((char*)&cmdBuffer[2 + rU8], 2);
				if(rShort != -100)
					sNumber[rU8 / 2] = (uint8_t)rShort;
				else
				{
					ret = -1;
					break;
				}
				
			}
			CopyBuff2FlashWrite( FlashDataPosTab[SERIAL_NUMBER].pos, &sNumber, FlashDataPosTab[SERIAL_NUMBER].size);
			break;

	}

	read_configuration_data(&configs);

	if(ret == -1)
		HAL_UART_Transmit(&huart1, (uint8_t*)"ERR\r\n", 5, 100);
	else if(ret == 0)
	{
		conf_flag = 1;
		HAL_UART_Transmit(&huart1, (uint8_t*)"OK\r\n", 4, 100);
	}
	return ret;
}


int handleGetFunc()
{
	uint8_t rU8;
	char rC8, buff[16];
	uint32_t rU32;
	switch(cmdBuffer[1])
	{
		case 'G':
			ReadFlash2Buff( FlashDataPosTab[GROUP].pos, &rU8, FlashDataPosTab[GROUP].size,0);
			hex2ch(rU8, buff);
			HAL_UART_Transmit(&huart1, (uint8_t*)"GG", 2, 100);
			HAL_UART_Transmit(&huart1, (uint8_t*)buff, 2, 100);
			HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, 100);
			break;

		case 'N':
			ReadFlash2Buff( FlashDataPosTab[NODE].pos, &rU8, FlashDataPosTab[NODE].size,0);
			hex2ch(rU8, buff);
			HAL_UART_Transmit(&huart1, (uint8_t*)"GN", 2, 100);
			HAL_UART_Transmit(&huart1, (uint8_t*)buff, 2, 100);
			HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, 100);
			break;

		case 'C':
			ReadFlash2Buff( FlashDataPosTab[CHANNEL].pos, &rU8, FlashDataPosTab[CHANNEL].size,0);
			hex2ch(rU8, buff);
			HAL_UART_Transmit(&huart1, (uint8_t*)"GC", 2, 100);
			HAL_UART_Transmit(&huart1, (uint8_t*)buff, 2, 100);
			HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, 100);
			break;

		case 'L':
			ReadFlash2Buff( FlashDataPosTab[DBG].pos, &rU8, FlashDataPosTab[DBG].size,0);
			hex2ch(rU8, buff);
			HAL_UART_Transmit(&huart1, (uint8_t*)"GL", 2, 100);
			HAL_UART_Transmit(&huart1, (uint8_t*)buff, 2, 100);
			HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, 100);
			break;

		case 'P':
			ReadFlash2Buff( FlashDataPosTab[TX_POWER].pos, &rU8, FlashDataPosTab[TX_POWER].size,0);
			hex2ch(rU8, buff);
			HAL_UART_Transmit(&huart1, (uint8_t*)"GP", 2, 100);
			HAL_UART_Transmit(&huart1, (uint8_t*)buff, 2, 100);
			HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, 100);
			break;

		case 'T':
			ReadFlash2Buff( FlashDataPosTab[TX_TIME].pos, &rU8, FlashDataPosTab[TX_TIME].size,0);
			hex2ch(rU8, buff);
			HAL_UART_Transmit(&huart1, (uint8_t*)"GT", 2, 100);
			HAL_UART_Transmit(&huart1, (uint8_t*)buff, 2, 100);
			HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, 100);
			break;

		case 'R':
			ReadFlash2Buff( FlashDataPosTab[RF_CALIBRATION].pos, &rU8, FlashDataPosTab[RF_CALIBRATION].size,0);
			ReadFlash2Buff( 5, (uint8_t*)&rU32, 2,0);
			hex2ch((rU32 >> 0) & 0xFF, buff);
			hex2ch((rU32 >> 8) & 0xFF, &buff[2]);
			HAL_UART_Transmit(&huart1, (uint8_t*)"GR", 2, 100);
			HAL_UART_Transmit(&huart1, (uint8_t*)buff, 4, 100);
			HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, 100);
			break;

		case 'J':
			ReadFlash2Buff( FlashDataPosTab[SLEEP_ON].pos, &rU8, FlashDataPosTab[SLEEP_ON].size,0);
			hex2ch(rU8, buff);
			HAL_UART_Transmit(&huart1, (uint8_t*)"GJ", 2, 100);
			HAL_UART_Transmit(&huart1, (uint8_t*)buff, 2, 100);
			HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, 100);
			break;

		case 'S':
			ReadFlash2Buff( 0, (uint8_t*)&rU32, 4,1);
			hex2ch(rU32 & 0xFF, &buff[0]);
			hex2ch((rU32 >> 8) & 0xFF, &buff[2]);
			hex2ch((rU32 >> 16) & 0xFF, &buff[4]);
			hex2ch((rU32 >> 24) & 0xFF, &buff[6]);
			ReadFlash2Buff( 4, (uint8_t*)&rU32, 4,1);
			hex2ch(rU32 & 0xFF, &buff[8]);
			hex2ch((rU32 >> 8) & 0xFF, &buff[10]);
			hex2ch((rU32 >> 16) & 0xFF, &buff[12]);
			hex2ch((rU32 >> 24) & 0xFF, &buff[14]);
		

			HAL_UART_Transmit(&huart1, (uint8_t*)"GS", 2, 100);
			HAL_UART_Transmit(&huart1, (uint8_t*)buff, 16, 100);
			HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, 100);
			break;
	}
	return 0;
}



int handleUart2Pkt()
{
	switch(cmdBuffer[0])
	{
	case 'D': // TEST
		break;
	case 'S':
		handleSetFunc();
		break;
	case 'G':
		handleGetFunc();
		break;
	case 'C':
		if ( cmdBuffer[1] == 'T' )
		{
			HAL_UART_Transmit(&huart1, (uint8_t*)"OK\r\n",4, 100);
		}
		break;
		
	case 'R':
		if ( cmdBuffer[1] == 'S' )
		{
			HAL_UART_Transmit(&huart1, (uint8_t*)"OK\r\n",4, 100);
			HAL_NVIC_SystemReset();
		}
		//HAL_UART_Transmit(&huart1, "OK", 2, 100);
		
		//handleReset();
		break;

	case 'M':
          	switch(cmdBuffer[1])
                {
                case 'C':
                    HAL_UART_Transmit(&huart1, "MC", 2, 100);
                    break;
                case 'E':
                    HAL_UART_Transmit(&huart1, "ME", 2, 100);
                    break;
                }
          	break;

	default:
		HAL_UART_Transmit(&huart1, (uint8_t*)"ERR\r\n", 5, 100);
		break;
	}

	return 0;
}

void uart2_receive_func()
{
 	if (uart_flag == 1)
	{
		if(recvUartPkt() > 0)
		{
			handleUart2Pkt();
			uart_flag = 0;
		}
	}
}


