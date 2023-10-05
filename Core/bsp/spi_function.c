/*
 * cc1120.c
 *
 *  Created on: 2021. 11. 30.
 *      Author: lucky
 */
#include "main.h"
#include "stm32l4xx_hal.h"
#include "cc112x_spi.h"
#include "spi_function.h"
#include "spi.h"
#include "cc1120_ref_reg.h"


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "chip_detect.h"


#include "arm-def.h"
radioChipType_t perRadioChipType;

#define CHIP_TYPE_CC1125   0x1125


#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

extern uint8_t  packetSemaphore;








/*******************************************************************************
*   @fn         cc112xregConfig
*
*   @brief      Write register settings as given by SmartRF Studio found in
*               cc112x_easy_link_reg_config.h
*
*   @param      freq - frequency channel (0-80)
*
*   @return     none
*/
#define CC112X_FREQ_BASE 0x00668000l   // frequency base is 410Mhz
#define CC112X_FREQ_STEP 0x4000        // frequency step is 1Mhz

void cc112xregConfig(uint8_t freq) {

    uint8_t writeByte;
    uint16_t i;
	uint32_t freq_code;
	freq_code = CC112X_FREQ_BASE + ((uint16_t)freq * CC112X_FREQ_STEP);// original code

    // Reset radio
	CC1120_SpiCmdStrobe(CC112X_SRES);

	HAL_Delay(100);

	CC1120_SpiCmdStrobe(CC112X_SRES);

    // Write registers to radio
	for(i = 0; i < (sizeof(preferredSettings)/sizeof(registerSetting_t)); i++) {
		writeByte = preferredSettings[i].data;
		if (preferredSettings[i].addr == CC112X_FREQ2) writeByte = (freq_code >> 16) & 0xff;
		else if (preferredSettings[i].addr == CC112X_FREQ1) writeByte = (freq_code >> 8) & 0xff;
		else if (preferredSettings[i].addr == CC112X_FREQ0) writeByte = freq_code & 0xff;
		CC1120_SpiWriteReg(preferredSettings[i].addr, &writeByte, 1);
    }
}



/*******************************************************************************
*   @fn         registerConfigChek
*
*   @brief      Write register settings as given by SmartRF Studio found in
*               cc112x_easy_link_reg_config.h
*
*   @param      none
*
*   @return     int (0: all registers config match, 1: one or more registers config didn't match)
*/
int cc112xConfigChek(uint8_t freq) {

	uint8_t readByte;
	uint8_t cmpByte;
	//buf[20];
	uint16_t i;
	uint32_t freq_code;

    freq_code = CC112X_FREQ_BASE + ((uint16_t)freq * CC112X_FREQ_STEP);


    // Read registers to radio
    for(i = 0; i < (sizeof(preferredSettings)/sizeof(registerSetting_t)); i++) {
        readByte = 0x00;
		cmpByte = preferredSettings[i].data;
		CC1120_SpiReadReg(preferredSettings[i].addr, &readByte, 1);
		if (preferredSettings[i].addr == CC112X_FREQ2) cmpByte = (freq_code >> 16) & 0xff;
		else if (preferredSettings[i].addr == CC112X_FREQ1) cmpByte = (freq_code >> 8) & 0xff;
		else if (preferredSettings[i].addr == CC112X_FREQ0) cmpByte = freq_code & 0xff;
		if (readByte != cmpByte) return 1;
    }
	return 0;

}


/******************************************************************************
 *
 *  @fn       cc112xpwrConfig(uint8_t pwrTx)
 *
 *  @brief
 *            Set Tx Power
 *
 *
 *
 * input parameters
 *
 * @param     pwrTx - Tx power value
 *
 * output parameters
 *
 * @return    none
 *
 */
void cc112xpwrConfig(uint8_t pwrTx) {
    uint8_t writeByte;
	writeByte = pwrTx;
	CC1120_SpiWriteReg(CC112X_PA_CFG2, &writeByte, 1);
}



#define RX_FIFO_ERROR       0x11
//uint16_t packetCounter = 0;





//uint8_t RxBuffer[128] = {0,};
//void RxMode()
//{
//	uint8_t marcStatus;
//	uint8_t temp_byte;
//	uint8_t rxBytes, i;
//	uint8_t rxBuffer[128] = {0,};
//	clock_time_t start , end;
//	if ( packetSemaphore == 1)//ISR_ACTION_REQUIRED )
//	{
//		//start = clock_time();
//		CC1120_SpiReadReg(CC112X_MARC_STATUS1, &temp_byte, 1);
//		if ( (temp_byte & 0x09) || (temp_byte & 0x0a) )
//		{
//			CC1120_SpiCmdStrobe(CC112X_SFRX);
//		}
//		else if ( temp_byte & 0x80 )
//		{
//			CC1120_SpiReadReg(CC112X_NUM_RXBYTES, &rxBytes, 1);
//
//			// Check that we have bytes in fifo
//			if ( rxBytes != 0 )
//			{
//				// Read marcstate to check for RX FIFO error
//				CC1120_SpiReadReg(CC112X_MARCSTATE, &marcStatus, 1);
//
//				// Mask out marcstate bits and check if we have a RX FIFO error
//				if((marcStatus & 0x1F) == RX_FIFO_ERROR)
//				{
//					// Flush RX Fifo
//					CC1120_SpiCmdStrobe(CC112X_SFRX);
//				}
//				else
//				{
//					// Read n bytes from rx fifo
//					CC1120_SpiReadRxFifo(rxBuffer, rxBytes);
//
//					//for(i = 0; i < rxBytes; i ++)
//					//	printf("rxBuffer[%d]: %d\n", i, rxBuffer[i]);
//
//					// Check CRC ok (CRC_OK: bit7 in second status byte)
//					// This assumes status bytes are appended in RX_FIFO
//					// (PKT_CFG1.APPEND_STATUS = 1.)
//					// If CRC is disabled the CRC_OK field will read 1
//					if(rxBuffer[rxBytes-1] & 0x80)
//					{
//						// Update packet counter
//						packetCounter++;
//					}
//				}
//			}
//		}
//		packetSemaphore = 0;
//		//end = clock_time();
//		CC1120_SpiCmdStrobe(CC112X_SIDLE);
//
//		do
//		{
//			CC1120_SpiReadReg(CC112X_MARCSTATE, &marcStatus, 1);
//		} while (marcStatus != 0x41);		// IDLE 모드 될때까지 확인
//
//
//		// Set radio back in RX
//		CC1120_SpiCmdStrobe(CC112X_SRX);
//
//	}
//}


uint8_t tx_complete = 0;




/******************************************************************************
 *
 *  @fn       cc112x_init(uint8_t freq, uint8_t pwr)
 *
 *  @brief
 *            Initialize/configure CC112x with given freq channel & tx power
 *
 *
 * input parameters
 *
 * @param     freq    - frequency channel
 * @param     pwr     - tx power
 *
 * output parameters
 *
 *
 * @return   none
 *
 */
void  cc112x_init(uint8_t freq, uint8_t pwr)
{
	uint16_t ii;
	uint8_t marcstate;
	ii = 0;
	do {
		cc112xregConfig(freq);
		if (( ii++ % 5) == 0) {
			/*** CC112X HW Reset ***/
			cc112x_hw_rst();
			//printf("CC1120 HW Reset.\r\n");

		}
		if ( ii > 15 ) {
		  //printf("CC1120 can not init.\r\n");
		  exit(1);
		}

	} while (cc112xConfigChek(freq));

	if (pwr>0x3e) pwr = 0x3e;
	cc112xpwrConfig(0x7f-pwr);

	manualCalibration();


	CC1120_SpiCmdStrobe(CC112X_SIDLE);

	// Flush RX FIFO
	CC1120_SpiCmdStrobe(CC112X_SFRX);

	// Flush TX FIFO
	CC1120_SpiCmdStrobe(CC112X_SFTX);

	do
	{
		CC1120_SpiReadReg(CC112X_MARCSTATE, &marcstate, 1);
	} while (marcstate != 0x41);		// IDLE 모드 될때까지 확인

	// Set radio in RX
	CC1120_SpiCmdStrobe(CC112X_SRX);

}



#define RSSI_OFFSET 74
signed char rssi_dbm(unsigned char raw_rssi)
{
  int16_t dbm = 0;

  if(raw_rssi >= 128) {
    dbm = (int16_t)((int16_t)(raw_rssi - 256) / 2) - RSSI_OFFSET;
  } else {
    dbm = (raw_rssi / 2) - RSSI_OFFSET;
  }
  return dbm;
}



void read_rssi(void)
{
	uint8_t raw;

	CC1120_SpiReadReg(CC112X_RSSI1, &raw, 1);
	rssi_dbm(raw);

}




