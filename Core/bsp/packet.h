/*
 * packet.h
 *
 *  Created on: 2022. 1. 31.
 *      Author: lucky
 */

#ifndef INC_PACKET_H_
#define INC_PACKET_H_

#include "global.h"
#include "ringbuf.h"

typedef struct _config_data
{
	uint8_t sensorID;   /* 0 */
	uint8_t NID;   /* 1 */
	uint8_t ch;    /* 2 */
	uint8_t pw;    /* 3 */
	uint8_t txP;   /* 4 *//* Normal data transmit period (hours) */
	int16_t fCal;  /* 5, 6 */ /* Frequency cal. value( * 10 Hz) -327.68 KHz ~ 327.67 KHz */
	uint8_t sleepOn;
	//uint8_t txRetry; /* 7 */
	uint8_t serialNumber[8]; /* 8, 9, 10, 11, 12, 13, 14, 15 *//* SERIAL NUMBER */
	uint8_t DBG_led;
	//uint8_t sensorCalStart; /* 16 */
} config_data;


extern ringbuffer_t rxrb;
extern config_data configs;
//#define rxbuf_ptr   &rxrb
void uartRingBuff_init();
void uart2_receive_func();
int read_configuration_data(config_data *c);
void CopyBuff2FlashWrite(uint16_t idx , uint8_t* data, uint8_t size);
#endif /* INC_PACKET_H_ */
