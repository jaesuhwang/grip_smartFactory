/*
 * tdma_table.h
 *
 *  Created on: 2022. 2. 7.
 *      Author: lucky
 */

#ifndef INC_TDMA_TABLE_H_
#define INC_TDMA_TABLE_H_

void tdma_table_clear(void);
uint8_t tdma_table_add(uint16_t node);
uint8_t tdma_table_del(uint16_t node);

uint16_t tdma_table_pos(uint16_t node);

#endif /* INC_TDMA_TABLE_H_ */
