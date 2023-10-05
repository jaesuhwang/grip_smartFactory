/*
 * backup.c
 *
 *  Created on: 2022. 3. 24.
 *      Author: luckyman
 */

#include "global.h"
#include "backup.h"
#include "rtc.h"

uint32_t flashwriteaddr = 0x08008000;
////uint32_t flashreaddata[4] = *(__IO uint32_t*)flashwriteaddr;
uint64_t flashreaddata[4] = { 0x1234567812345678, 0x12345678,0x12345678,0x12345678 };
//Read_ST_Flash(flashwriteaddr,&flashreaddata, 4 );



void rtc_write_backup_reg(uint32_t BackupRegister, uint32_t data) {
    RTC_HandleTypeDef RtcHandle;
    RtcHandle.Instance = RTC;
    HAL_PWR_EnableBkUpAccess();
    HAL_RTCEx_BKUPWrite(&RtcHandle, BackupRegister, data);
    HAL_PWR_DisableBkUpAccess();
}

uint32_t rtc_read_backup_reg(uint32_t BackupRegister) {
    RTC_HandleTypeDef RtcHandle;
    RtcHandle.Instance = RTC;
    return HAL_RTCEx_BKUPRead(&RtcHandle, BackupRegister);
}

//////////////////////////////////////////////////////////////////////////////////


void Read_ST_Flash(uint32_t addr, uint32_t*ptr, uint16_t nword)
{
	for ( uint16_t i=0; i<nword; i++)
	{
		ptr[i] = *(__IO uint32_t *)addr;
		addr = addr + 4;
	}
}


uint8_t Write_ST_Flash(uint32_t addr, uint64_t*ptr , uint16_t ndword)
{
	uint8_t page;
	uint32_t PAGEError = 0 ;
	FLASH_EraseInitTypeDef EraseInitStruct;

	if ( ndword > 512 )
	{
		return 1;
	}

	HAL_FLASH_Unlock();

	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

	page = ( addr - FLASH_BASE ) / FLASH_PAGE_SIZE;

	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.Banks = FLASH_BANK_1;
	EraseInitStruct.Page = page;
	EraseInitStruct.NbPages = 1;

	if ( HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError)!= HAL_OK)
	{
		// error
		return 1;
	}
	for ( uint16_t i = 0 ; i < ndword; i++)
	{
		if ( HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr, ptr[i]) == HAL_OK )
		{
			addr = addr + 8;
		}
		else
		{
			//error
			return 1;
		}
	}
	HAL_FLASH_Lock();
	return 0;
}



