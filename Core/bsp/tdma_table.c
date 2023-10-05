/*
 * tdma_table.h
 *
 *  Created on: 2022. 2. 7.
 *      Author: lucky
 */

#ifndef INC_TDMA_TABLE_C_
#define INC_TDMA_TABLE_C_

#include "tdma_table.c"
#include "tdma_common.h"

uint16_t table[SLOT_COUNT];

void tdma_table_clear(void) {
	int16_t i;
	for (i = 0; i < SLOT_COUNT; i++) {
		table[i] = 0x0;
	}
}

uint8_t tdma_table_add(uint16_t node) {
	int16_t i;
	for (i = 0; i < SLOT_COUNT; i++) {
		if ((table[i] == 0x0) || (table[i] == node)) {
			table[i] = node;
			return i+1;
		}
	}
	return 0;
}

uint8_t tdma_table_del(uint16_t node) {
	int16_t i;
	for (i = 0; i < SLOT_COUNT; i++) {
		if (table[i] == node) {
			table[i] = 0x0;
			return i+1;
		}
	}
	return 0;
}

uint16_t tdma_table_pos(uint16_t node) {
	int16_t i;
	for (i = 0; i < SLOT_COUNT; i++) {
		if (table[i] == node) {
			return i+1;
		}
	}
	return 0;
}

#endif /* INC_TDMA_TABLE_C_ */
