/*
 * backup.h
 *
 *  Created on: 2022. 3. 24.
 *      Author: luckyman
 */

#ifndef BACKUP_H_
#define BACKUP_H_

#include "stm32l4xx_hal.h"

void rtc_write_backup_reg(uint32_t BackupRegister, uint32_t data);
uint32_t rtc_read_backup_reg(uint32_t BackupRegister);

void Read_ST_Flash(uint32_t addr, uint32_t*ptr, uint16_t nword);
uint8_t Write_ST_Flash(uint32_t addr, uint64_t*ptr , uint16_t ndword);



#endif /* BACKUP_H_ */
