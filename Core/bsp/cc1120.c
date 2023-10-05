/*
 * cc1120.c
 *
 *  Created on: 2022. 2. 9.
 *      Author: lucky
 */



#include <stdio.h>
#include <stdlib.h>

#include "cc1120.h"

#include "spi.h"
#include "usart.h"


#include "cc1120_ref_reg.h"
#include "cc112x_spi.h"

#include "packet.h"
#include "urm37_distance.h"

#define FREQ_SEPERATE_VAL      0.5F

uint8_t old_temp;
uint8_t new_temp;

extern TIM_HandleTypeDef htim15;

uint32_t sync_word_time;
uint8_t g_channel;
float g_freq;
uint16_t (*gdo0_cb)(void);
uint16_t (*gdo2_cb)(void);

uint8_t txPowerTeb;

void cc1120_gdo0_register_callback(uint16_t (*cb)(void))
{
  gdo0_cb = cb;
}

void cc1120_gdo2_register_callback(uint16_t (*cb)(void))
{
  gdo2_cb = cb;
}


void cc112x_hw_rst(void) {
  HAL_GPIO_WritePin(CC1120_RST_PORT, CC1120_RST_PIN, GPIO_PIN_RESET);
  HAL_Delay(20);
  HAL_GPIO_WritePin(CC1120_RST_PORT, CC1120_RST_PIN, GPIO_PIN_SET);
  HAL_Delay(5);
}



uint8_t CC1120_SpiRead(uint8_t c)
{
	uint8_t rc;
	HAL_StatusTypeDef status = HAL_OK;
	status = HAL_SPI_TransmitReceive(&hspi1, &c, &rc, 1, 20);
	if (status != HAL_OK)
	{
		return 0xff;
	}
	return (rc);
}

uint8_t CC1120_SpiWrite(uint8_t c)
{
	HAL_StatusTypeDef status = HAL_OK;
	status = HAL_SPI_Transmit(&hspi1, &c, 1, 20);
	if (status != HAL_OK)
	{
		return 0xff;
	}
	return c;
}

uint8_t CC1120_SpiCmdStrobe(uint8_t cmd)
{
	HAL_StatusTypeDef status = HAL_OK;

	uint8_t rc;
	SPI1_NSS_LOW();
	//old_temp = 9;


	//HAL_Delay(1);

	#if 0
	if (new_temp != old_temp)
	{
	old_temp = new_temp;
	HAL_UART_Transmit(&huart1, (uint8_t*)"t9\r\n", 5, 10);
	}
	#endif
	//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET);
	
	// status = HAL_SPI_TransmitReceive(&hspi1, &cmd, &rc, 1, 20);  // 원래 
	status = HAL_SPI_TransmitReceive(&hspi1, &cmd, &rc, 1, 20);    // 테스트

	if (status != HAL_OK)
	{
		return 0xFF;           
	}
	
	SPI1_NSS_HIGH();
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET);
	return (rc);
}
// ---------------------------------------

void CC1120_ReadWriteBurstSingle(uint16_t addr, uint8_t *pData, uint8_t len)
{
	unsigned i;
	/* Communicate len number of bytes: if RX - the procedure sends 0x00 to push bytes from slave*/
	if (addr & RADIO_READ_ACCESS)
	{
		if (addr & RADIO_BURST_ACCESS)
		{
			for (i = 0; i < len; i++)
			{
				*pData++ = CC1120_SpiRead(0); /* Possible to combining read and write as one access type */
			}
		}
		else
		{
			*pData = CC1120_SpiRead(0);
		}
	}
	else
	{
		if (addr & RADIO_BURST_ACCESS)
		{
			/* Communicate len number of bytes: if TX - the procedure doesn't overwrite pData */
			for (i = 0; i < len; i++)
			{
				CC1120_SpiWrite(*pData++);
			}
		}
		else
		{
			CC1120_SpiWrite(*pData);
		}
	}
	return;
}

uint8_t CC1120_8BitRegAccess(uint8_t accessType, uint8_t addrByte, uint8_t *pData, uint8_t len)
{
	char readValue;

	/* Pull CS_N low and wait for SO to go low before communication starts */
	SPI1_NSS_LOW();
	/* send register address byte */
	readValue = CC1120_SpiRead(accessType | addrByte);
	CC1120_ReadWriteBurstSingle(accessType | addrByte, pData, len);
	SPI1_NSS_HIGH();
	/* return the status byte value */
	return (readValue);
}

uint8_t CC1120_16BitRegAccess(uint8_t accessType, uint8_t extAddr, uint8_t regAddr, uint8_t *pData, uint8_t len)
{
	uint8_t readValue;

	SPI1_NSS_LOW();
	/* send extended address byte with access type bits set */
	readValue = CC1120_SpiRead(accessType | extAddr);
	CC1120_SpiWrite(regAddr);
	/* Communicate len number of bytes */
	CC1120_ReadWriteBurstSingle(accessType | extAddr, pData, len);
	SPI1_NSS_HIGH();
	/* return the status byte value */
	return (readValue);
}

uint8_t CC1120_SpiWriteReg(uint16_t addr, uint8_t *pData, uint8_t len)
{
	uint8_t tempExt = (uint8_t) (addr >> 8);
	uint8_t tempAddr = (uint8_t) (addr & 0x00FF);
	uint8_t rc;

	/* Checking if this is a FIFO access - returns chip not ready */
	if ((CC112X_SINGLE_TXFIFO <= tempAddr) && (tempExt == 0))
		return STATUS_CHIP_RDYn_BM;

	/* Decide what register space is accessed */
	if (!tempExt)
	{
		rc = CC1120_8BitRegAccess((RADIO_BURST_ACCESS | RADIO_WRITE_ACCESS), tempAddr, pData, len);
	}
	else if (tempExt == 0x2F)
	{
		rc = CC1120_16BitRegAccess((RADIO_BURST_ACCESS | RADIO_WRITE_ACCESS), tempExt, tempAddr, pData, len);
	}
	return (rc);
}

uint8_t CC1120_SpiReadReg(uint16_t addr, uint8_t *pData, uint8_t len)
{
	uint8_t tempExt = (uint8_t) (addr >> 8);
	uint8_t tempAddr = (uint8_t) (addr & 0x00FF);
	uint8_t rc;

	/* Checking if this is a FIFO access -> returns chip not ready  */
	if ((CC112X_SINGLE_TXFIFO <= tempAddr) && (tempExt == 0))
		return STATUS_CHIP_RDYn_BM;

	/* Decide what register space is accessed */
	if (!tempExt)
	{
		rc = CC1120_8BitRegAccess((RADIO_BURST_ACCESS | RADIO_READ_ACCESS), tempAddr, pData, len);
	}
	else if (tempExt == 0x2F)
	{
		rc = CC1120_16BitRegAccess((RADIO_BURST_ACCESS | RADIO_READ_ACCESS), tempExt, tempAddr, pData, len);
	}
	return (rc);
}

uint8_t CC1120_SpiWriteTxFifo(uint8_t *pData, uint8_t len)
{
	uint8_t rc;
	rc = CC1120_8BitRegAccess(0x00, CC112X_BURST_TXFIFO, pData, len);
	return (rc);
}

uint8_t CC1120_SpiReadRxFifo(uint8_t * pData, uint8_t len)
{
	uint8_t rc;
	rc = CC1120_8BitRegAccess(0x00, CC112X_BURST_RXFIFO, pData, len);
	return (rc);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GetPartAndVersion()
{
	uint8_t readData;
	uint8_t buffertt[32] = {0x00,};

	CC1120_SpiReadReg(CC112X_PARTNUMBER, &readData, 1);
	sprintf(buffertt, "PARTNUMBER %02x\r\n", readData);
	HAL_UART_Transmit(&huart1, buffertt, strlen(buffertt), 100);

	CC1120_SpiReadReg(CC112X_PARTVERSION, &readData, 1);
	sprintf(buffertt, "partversion %02x\r\n", readData);
	HAL_UART_Transmit(&huart1, buffertt, strlen(buffertt), 100);
	//printf("CC1120 part ver: 0x%02X\n", readData);
}


/*******************************************************************************
* @fn          registerConfig
*
* @brief       Write register settings as given by SmartRF Studio found in
*              cc112x_easy_link_reg_config.h
*
* @param       none
*
* @return      none
*/
uint8_t readByte[50];
char temdata[100];
void registerConfig(void)
{
    uint8_t writeByte;
	
	uint8_t temp_data;
//    uint8_t data[4] = {0x00,};
    // Reset radio
    CC1120_SpiCmdStrobe(CC112X_SRES);

    // Write registers to radio
    for(uint16_t i = 0; i < (sizeof  preferredSettings/sizeof(registerSetting_t)); i++) {
      writeByte =  preferredSettings[i].data;
      CC1120_SpiWriteReg( preferredSettings[i].addr, &writeByte, 1);
      // 2022 04 01
      HAL_Delay(1);
    }


	for(uint16_t i = 0; i < (sizeof  preferredSettings/sizeof(registerSetting_t)); i++) {
      
	  CC1120_SpiReadReg( preferredSettings[i].addr,&readByte[i] ,1 );
      HAL_Delay(1);
    }




	HAL_UART_Transmit(&huart1, (uint8_t*)"data\r\n", 6, 10);	
	HAL_UART_Transmit(&huart1, (uint8_t*)readByte, 47, 10);
	HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, 10);
	HAL_UART_Transmit(&huart1, (uint8_t*)"end\r\n", 5, 10);

	
   

// 여기 경운이
}



uint8_t CC1120_setFrequency(float centre) {
  // From section 9.12 (lo_divider in Table 30):
  //   fcarrier = fxosc / lo_divider * 2^16 * FREQ

  uint32_t FREQ;
  uint8_t bandselect, lo_divider;
  float fxosc = 32.0;

  if (centre >= 820.0 && centre <= 960.0) {
    lo_divider = 4;
    bandselect = 0x02;//0b0010;
  }

  if (centre >= 410.0 && centre <= 480.0) {
    lo_divider = 8;
    bandselect = 0x04;//0b0100;
  }

  if (centre >= 273.3 && centre <= 320.0) {
    lo_divider = 12;
    bandselect = 0x06;//0b0110;
  }

  if (centre >= 205.0 && centre <= 240.0) {
    lo_divider = 16;
    bandselect = 0x08;//0b1000;
  }

  if (centre >= 164.0 && centre <= 192.0) {
    lo_divider = 20;
    bandselect = 0x0a;//0b1010;
  }

  if (centre >= 136.7 && centre <= 160.0) {
    lo_divider = 24;
    bandselect = 0x0b;//0b1011;
  }

  FREQ = (uint32_t)(centre * 65536 * lo_divider / fxosc);

  // Some trivial checks
  if (FREQ & 0xff000000)
    return 0;

  uint8_t freq2, freq1,freq0;

  freq2 = (FREQ >> 16) & 0xff;
  freq1 = (FREQ >> 8) & 0xff;
  freq0 = FREQ & 0xff;
  CC1120_SpiWriteReg(CC112X_FREQ2, &freq2, 1);
  CC1120_SpiWriteReg(CC112X_FREQ1, &freq1, 1);
  CC1120_SpiWriteReg(CC112X_FREQ0, &freq0, 1);

//  spiWriteRegister(RH_CC1120_REG_2F0C_FREQ2, (FREQ >> 16) & 0xff);
//  spiWriteRegister(RH_CC1120_REG_2F0D_FREQ1, (FREQ >> 8) & 0xff);
//  spiWriteRegister(RH_CC1120_REG_2F0E_FREQ0, FREQ & 0xff);
  uint8_t fs_cfg;
  CC1120_SpiReadReg(CC112X_FS_CFG, &fs_cfg, 1);
  fs_cfg = (fs_cfg & ~0x0F/*~0b1111*/) | bandselect;
  CC1120_SpiWriteReg(CC112X_FS_CFG, &fs_cfg, 1);
//  uint8_t fs_cfg = CC1120_SpiReadReg(RH_CC1120_REG_0021_FS_CFG);
//  fs_cfg = (fs_cfg & ~0x0F/*~0b1111*/) | bandselect;
//  CC1120_SpiWriteReg(CC112X_FS_CFG, fs_cfg);

  // Radio is configured to calibrate automatically whenever it enters RX or
  // TX mode so no need to check for PLL lock here
  return 1;
}




/*******************************************************************************
*   @fn         manualCalibration
*
*   @brief      Calibrates radio according to CC112x errata
*
*   @param      none
*
*   @return     none
*/

#define VCDAC_START_OFFSET 2
#define FS_VCO2_INDEX 0
#define FS_VCO4_INDEX 1
#define FS_CHP_INDEX 2

void manualCalibration()
{

	uint8_t original_fs_cal2;
	uint8_t calResults_for_vcdac_start_high[3];
	uint8_t calResults_for_vcdac_start_mid[3];
	uint8_t marcstate;
	uint8_t writeByte;

	// 1) Set VCO cap-array to 0 (FS_VCO2 = 0x00)
	writeByte = 0x00;
	CC1120_SpiWriteReg(CC112X_FS_VCO2, &writeByte, 1);

	// 2) Start with high VCDAC (original VCDAC_START + 2):
	CC1120_SpiReadReg(CC112X_FS_CAL2, &original_fs_cal2, 1);
	writeByte = original_fs_cal2 + VCDAC_START_OFFSET;
	CC1120_SpiWriteReg(CC112X_FS_CAL2, &writeByte, 1);

	// 3) Calibrate and wait for calibration to be done
	//   (radio back in IDLE state)
	CC1120_SpiCmdStrobe(CC112X_SCAL);

	do
	{
		CC1120_SpiReadReg(CC112X_MARCSTATE, &marcstate, 1);
	} while (marcstate != 0x41);

	// 4) Read FS_VCO2, FS_VCO4 and FS_CHP register obtained with
	//    high VCDAC_START value
	CC1120_SpiReadReg(CC112X_FS_VCO2, &calResults_for_vcdac_start_high[FS_VCO2_INDEX], 1);
	CC1120_SpiReadReg(CC112X_FS_VCO4, &calResults_for_vcdac_start_high[FS_VCO4_INDEX], 1);
	CC1120_SpiReadReg(CC112X_FS_CHP, &calResults_for_vcdac_start_high[FS_CHP_INDEX], 1);

	// 5) Set VCO cap-array to 0 (FS_VCO2 = 0x00)
	writeByte = 0x00;
	CC1120_SpiWriteReg(CC112X_FS_VCO2, &writeByte, 1);

	// 6) Continue with mid VCDAC (original VCDAC_START):
	writeByte = original_fs_cal2;
	CC1120_SpiWriteReg(CC112X_FS_CAL2, &writeByte, 1);

	// 7) Calibrate and wait for calibration to be done
	//   (radio back in IDLE state)
	CC1120_SpiCmdStrobe(CC112X_SCAL);

	do
	{
		CC1120_SpiReadReg(CC112X_MARCSTATE, &marcstate, 1);
	} while (marcstate != 0x41);

	// 8) Read FS_VCO2, FS_VCO4 and FS_CHP register obtained
	//    with mid VCDAC_START value
	CC1120_SpiReadReg(CC112X_FS_VCO2, &calResults_for_vcdac_start_mid[FS_VCO2_INDEX], 1);
	CC1120_SpiReadReg(CC112X_FS_VCO4, &calResults_for_vcdac_start_mid[FS_VCO4_INDEX], 1);
	CC1120_SpiReadReg(CC112X_FS_CHP, &calResults_for_vcdac_start_mid[FS_CHP_INDEX], 1);

	// 9) Write back highest FS_VCO2 and corresponding FS_VCO
	//    and FS_CHP result
	if (calResults_for_vcdac_start_high[FS_VCO2_INDEX] > calResults_for_vcdac_start_mid[FS_VCO2_INDEX])
	{
		writeByte = calResults_for_vcdac_start_high[FS_VCO2_INDEX];
		CC1120_SpiWriteReg(CC112X_FS_VCO2, &writeByte, 1);
		writeByte = calResults_for_vcdac_start_high[FS_VCO4_INDEX];
		CC1120_SpiWriteReg(CC112X_FS_VCO4, &writeByte, 1);
		writeByte = calResults_for_vcdac_start_high[FS_CHP_INDEX];
		CC1120_SpiWriteReg(CC112X_FS_CHP, &writeByte, 1);
	}
	else
	{
		writeByte = calResults_for_vcdac_start_mid[FS_VCO2_INDEX];
		CC1120_SpiWriteReg(CC112X_FS_VCO2, &writeByte, 1);
		writeByte = calResults_for_vcdac_start_mid[FS_VCO4_INDEX];
		CC1120_SpiWriteReg(CC112X_FS_VCO4, &writeByte, 1);
		writeByte = calResults_for_vcdac_start_mid[FS_CHP_INDEX];
		CC1120_SpiWriteReg(CC112X_FS_CHP, &writeByte, 1);
	}
}



/* Values in this table must be ored with the PA_CFG2 register to account for
 * pa_shape_en
 * Formula: paPowerRamp[index] = dec2hex((wanted_dBm_level+18)*2-1)
 */
static const uint8_t paPowerRamp[] =
{
#if 0
  0x03, /* -16 dBm - lowest power       - index 0 */
  0x3F, /*  14 dBm - highest power      - index 1 */
  0x0F, /* -10 dBm - next lowest power  - index 2 */
  0x19, /*  -5 dBm -                    - index 3 */
  0x23, /*   0 dBm -                    - index 4 */
  0x2D, /*   5 dBm -                    - index 5 */
  0x37  /*  10 dBm - next highest power - index 6 */
#endif
  0x43, //-11dBm
  0x4f, // -6dBm
  0x56, // -3dBm
  0x5d, //  0dBm
  0x5f, //  1dBm
  0x62, //  2dBm
  0x64, //  3dBm
  0x66, //  4dBm
  0x69, //  5dBm
  0x6b, //  6dBm
  0x6d, //  7dBm
  0x6f, //  8dBm
  0x72, //  9dBm
  0x74, // 10dBm
  0x77, // 11dBm
  0x79, // 12dBm
  0x7b, // 13dBm
  0x7d, // 14dBm
  0x7f  // 15dBm
};


/******************************************************************************
* @fn          perCC112xSetOutputPower
*
* @brief       Configures the output power of CC112x according to the provided
*              index:
*              0 = -16 dBm
*              1 =  14 dBm
*              2 = -10 dBm
*              3 = -5 dBm
*              4 =  0 dBm
*              5 =  5 dBm
*              6 = 10 dBm
*              NOTE: for PG2.0 pa_shape_en and pa_power_ramp has swapped
*                    position
*
*
*
* input parameters
*
* @param       index - index to table <=> wanted output level
*
* output parameters
*
* @return      void
*/
void perCC112xSetOutputPower(uint8_t index)
{
  uint8_t level;
#if 0
  /* Reading the PA_CFG2 value to account for pa_shape_en */
  CC1120_SpiReadReg(CC112X_PA_CFG2,&level,1);
  /* Saving pa_shape_en */
  level &= 0x40;
  /* Oring in the PA power ramp value */
  level |= paPowerRamp[index];
  /* Updating PA_CFG2 register with its' new value */
  CC1120_SpiWriteReg(CC112X_PA_CFG2,&level,1);
#endif
  level = paPowerRamp[index];
  CC1120_SpiWriteReg(CC112X_PA_CFG2,&level,1);

  return;
}


void cc1120_cmd_idle(void)
{
  uint8_t state,sate2;
  uint8_t retryCnt;
  //sate2 = CC1120_SpiCmdStrobe(CC112X_SNOP);
  //printf("sts =%02x\r\n",sate2);
  //state = sate2 & CC112X_STATUS_MASK;
  state = CC1120_SpiCmdStrobe(CC112X_SNOP) & CC112X_STATUS_MASK;

  switch (state)
  {
    case CC112X_STATE_RXFIFO_ERROR:
    	cc1120_cmd_flush_rx();
      break;
    case CC112X_STATE_TXFIFO_ERROR:
    	cc1120_cmd_flush_tx();
      break;
    default:
    	CC1120_SpiCmdStrobe(CC112X_SIDLE);
  }

  while ((CC1120_SpiCmdStrobe(CC112X_SNOP) & CC112X_STATUS_MASK) != CC112X_STATE_IDLE) {
	  CC1120_SpiCmdStrobe(CC112X_SIDLE);
  }
}

#define WAIT_STATUS(status) \
		__NOP();__NOP();		\
    while ( (cc1120_cmd_nop() & CC112X_STATUS_MASK) != status) ;

void cc1120_cmd_calibrate(void)
{
	cc1120_cmd_idle();
	CC1120_SpiCmdStrobe(CC112X_SCAL);
//	new_temp = 7;
//	if (new_temp != old_temp)
//	{
//		old_temp = new_temp;
//		HAL_UART_Transmit(&huart1, (uint8_t*)"t7\r\n", 5, 10);
//	}
	
  	WAIT_STATUS(CC112X_STATE_IDLE);
}
extern uint8_t MyNodeID ;
void PacketSend(uint8_t destAddr,uint8_t srcAddr, uint8_t frame )
{
	uint8_t txBuf[32] = {0x00,};
	uint8_t rc, statusByte;
	uint8_t marcstate;
	uint8_t val;

	CC1120_SpiCmdStrobe(CC112X_SIDLE);
					//Wait for radio to enter IDLE state before issuing the SPWD command
	do
	{

		rc = CC1120_SpiCmdStrobe(CC112X_SNOP);
		__NOP();
	} while ((rc & 0xF0) != 0x00);
//
//	//CC1120_SpiReadReg(CC112X_RSSI, &val, 1);


#if 1
	txBuf[0] = 0x07;
	txBuf[1] = destAddr;
	txBuf[2] = srcAddr;
	txBuf[3] = frame;

	if ( MyNodeID == 0x01)
	{
		txBuf[4] = 0xAA;
			txBuf[5] = 0xAA;
			txBuf[6] = 0xAA;
	}
	else if ( MyNodeID == 0x02)
		{
		txBuf[4] = 0xBB;
			txBuf[5] = 0xBB;
			txBuf[6] = 0xBB;
		}
	else if ( MyNodeID == 0x03)
		{
		txBuf[4] = 0xCC;
			txBuf[5] = 0xCC;
			txBuf[6] = 0xCC;
		}
	else if ( MyNodeID == 0x04)
		{
		txBuf[4] = 0xDD;
			txBuf[5] = 0xDD;
			txBuf[6] = 0xDD;
		}
	else if ( MyNodeID == 0x05)
		{
		txBuf[4] = 0xEE;
			txBuf[5] = 0xEE;
			txBuf[6] = 0xEE;
		}



	CC1120_SpiReadReg(CC112X_MARC_STATUS1, &statusByte, 1);
	if (statusByte == 0x07 ||  statusByte == 0x08)
	{
		//Flush FIFO
		CC1120_SpiCmdStrobe(CC112X_SFTX);
	}
	else if ( statusByte == 0 )
	{
		send_packet(txBuf);
		//return ;
	}
//	CC1120_SpiCmdStrobe(CC112X_SIDLE);
//	// Wait for radio to enter IDLE state.
//
//	do
//	{
//		CC1120_SpiReadReg(CC112X_MARCSTATE, &marcstate, 1);
//	} while (marcstate != 0x41);		// IDLE 모드 ?��?��까�?? ?��?��
//	 //Set radio back in RX
//	CC1120_SpiCmdStrobe(CC112X_SRX);
#endif
}


void PacketSend2(uint8_t* msg)
{
	uint8_t txBuf[32] = {0x00,};
	uint8_t rc, statusByte;
	uint8_t marcstate;
	uint8_t val;

	CC1120_SpiCmdStrobe(CC112X_SIDLE);
					//Wait for radio to enter IDLE state before issuing the SPWD command
	do
	{
		rc = CC1120_SpiCmdStrobe(CC112X_SNOP);
	//__no_operation();
	} while ((rc & 0xF0) != 0x00);
//
//	//CC1120_SpiReadReg(CC112X_RSSI, &val, 1);



	CC1120_SpiReadReg(CC112X_MARC_STATUS1, &statusByte, 1);
	if (statusByte == 0x07 ||  statusByte == 0x08)
	{
		//Flush FIFO
		CC1120_SpiCmdStrobe(CC112X_SFTX);
	}
	else if ( statusByte == 0 )
	{
		send_packet(msg);
		//return ;
	}

	CC1120_SpiCmdStrobe(CC112X_SIDLE);
	// Wait for radio to enter IDLE state.

	do
	{
		CC1120_SpiReadReg(CC112X_MARCSTATE, &marcstate, 1);
	} while (marcstate != 0x41);		// IDLE 모드 ?��?��까�?? ?��?��
	 //Set radio back in RX
	CC1120_SpiCmdStrobe(CC112X_SRX);
}




/******************************************************************************
 *
 *  @fn       uint8_t send_packet(uint8_t * sendBuffer)
 *
 *  @brief
 *            Send packet data from given buffer (sendBuffer)
 *			  the first byte of sendBuffer is data length
 *            data length must > 0
 *
 * input parameters
 *
 * @param     sendBuffer   - Buffer of data send
 *
 * output parameters
 *
 *
 * @return   send result status, 0 = success, another value = fail
 *
 */
uint8_t send_packet(uint8_t * sendBuffer) {
	uint8_t temp_byte;
	uint8_t len;
	len = *sendBuffer;
	uint8_t marcstate;
//	if (len == 0) return 0;

//	CC1120_SpiReadReg(CC112X_MARCSTATE, &temp_byte, 1);  // rx mode 이다 하지말자
//	if (temp_byte != 0x6d)
//		return 0;

//	CC1120_SpiReadReg(CC112X_RSSI0, &temp_byte, 1);
//	if (temp_byte & 0x40) {
//		len =  0; // if RSSI present from another
//		return 0;
//	}

//	CC1120_SpiCmdStrobe(CC112X_SIDLE);
//	do
//	{
//		CC1120_SpiReadReg(CC112X_MARCSTATE, &marcstate, 1);
//	} while (marcstate != 0x41);		// IDLE 모드 될때까지 확인

//	CC1120_SpiCmdStrobe(CC112X_SFTX);					/*  SFTX    - Flush the TX FIFO buffer. */
//	CC1120_SpiCmdStrobe(CC112X_SFRX);					/*  SFRX    - Flush the RX FIFO buffer. */

	//temp_byte = 0x06;							// 확인해봐야지
	//CC1120_SpiWriteReg(CC112X_IOCFG3, &temp_byte, 1);

	// Write packet to TX FIFO
	CC1120_SpiWriteTxFifo(sendBuffer, len+1); //

	// Strobe TX to send packet
	CC1120_SpiCmdStrobe(CC112X_STX);
	do {
		//os_dly_wait (1);
		CC1120_SpiReadReg(CC112X_MARC_STATUS1, &temp_byte, 1);
		//printf("marcstate: %02x\r\n", marcState);
	} while (( temp_byte) == 0);
//	tx_complete = 1;
	if ( temp_byte & 0x40) *sendBuffer = 0;  // 최대 길이 필터링으로 인해 패킷이 삭제됨

	//CC1120_SpiCmdStrobe(CC112X_SRX);


	return temp_byte;
}

void send_packet2(uint8_t * sendBuffer, uint8_t len) {
	uint8_t temp_byte;
	uint8_t marcstate;

//	CC1120_SpiReadReg(CC112X_MARCSTATE, &temp_byte, 1);  // rx mode 이다 하지말자
//	if (temp_byte != 0x6d)
//		return 0;

//	CC1120_SpiReadReg(CC112X_RSSI0, &temp_byte, 1);
//	if (temp_byte & 0x40) {
//		len =  0; // if RSSI present from another
//		return 0;
//	}

	CC1120_SpiCmdStrobe(CC112X_SIDLE);
	do
	{
		//__NOP();
		CC1120_SpiReadReg(CC112X_MARCSTATE, &marcstate, 1);
	} while (marcstate != 0x41);		// IDLE 모드 될때까지 확인

	CC1120_SpiCmdStrobe(CC112X_SFTX);					/*  SFTX    - Flush the TX FIFO buffer. */
	CC1120_SpiCmdStrobe(CC112X_SFRX);					/*  SFRX    - Flush the RX FIFO buffer. */
	//temp_byte = 0x06;							// 확인해봐야지
	//CC1120_SpiWriteReg(CC112X_IOCFG3, &temp_byte, 1);

	// Write packet to TX FIFO
	CC1120_SpiWriteTxFifo(sendBuffer, len+1); //

	// Strobe TX to send packet
	CC1120_SpiCmdStrobe(CC112X_STX);

//	do {
//		//os_dly_wait (1);
//		CC1120_SpiReadReg(CC112X_MARC_STATUS1, &temp_byte, 1);
//		//printf("marcstate: %02x\r\n", marcState);
//	} while (( temp_byte) == 0x00);
////	tx_complete = 1;
//	//if ( temp_byte & 0x40) *sendBuffer = 0;  // 최대 길이 필터링으로 인해 패킷이 삭제됨
//
//	CC1120_SpiCmdStrobe(CC112X_SIDLE);
//	do
//	{
//		CC1120_SpiReadReg(CC112X_MARCSTATE, &marcstate, 1);
//	} while (marcstate != 0x41);		// IDLE 모드 될때까지 확인
//
//	CC1120_SpiCmdStrobe(CC112X_SRX);

	//return temp_byte;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if ( GPIO_Pin == GPIO0_PIN)
	{
		if (gdo0_cb != 0x0)
		{
			gdo0_cb();
		}

	}
	if ( GPIO_Pin == GPIO2_PIN)
	{
		if (gdo2_cb != 0x0)
		{
			gdo2_cb();
		}
	}
	if( GPIO_Pin == GPIO_PIN_14)
	{
		if ( ultra_busy )
    	{
			if(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14) )
			{
				HAL_TIM_Base_Start(&htim15);
				
			}
      		else
			{
				ultraTimerStop();
				ultra_busy = 0;
			}
	
    	}
		 
	}
}

extern config_data configs;

void cc1120_freq_set(uint8_t sensor_id)
{
	switch (sensor_id)
	{
		case 0:
		{
			g_freq = 868;
		}
		break;
		case 1:
		{
			g_freq = 940;
		}
		break;
		default:
		{
			g_freq = 940;
		}
		break;
	}
}

#if PC_TEST_MODE
void cc1120_init(uint8_t channel)
#else
void cc1120_init()
#endif
{
	gdo0_cb = 0x0;
	gdo2_cb = 0x0;

	cc112x_hw_rst();

	GetPartAndVersion();

	registerConfig();
	#if PC_TEST_MODE
	
	
	#else
	//CC1120_setFrequency(g_freq + ( (g_channel-1) * 0.5) );
	CC1120_setFrequency(940.000);
	perCC112xSetOutputPower(18);
	//perCC112xSetOutputPower(txPowerTeb);
	#endif
	//cc112xConfigChek(23);
	manualCalibration();

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_SET);
	// new_temp = 2;
    // if(new_temp != old_temp)
    // {
    //     old_temp = new_temp;
    //     HAL_UART_Transmit(&huart1, (uint8_t*)"t2\r\n", 5, 10);
    // } 
    
	
}
