/*
 * cc1120.h
 *
 *  Created on: 2021. 11. 30.
 *      Author: lucky
 */

#ifndef INC_SPI_FUNCTION_H_
#define INC_SPI_FUNCTION_H_



void cc112xregConfig(uint8_t freq) ;
int cc112xConfigChek(uint8_t freq) ;
void cc112xpwrConfig(uint8_t pwrTx);

void  cc112x_init(uint8_t freq, uint8_t pwr);
void cc112x_run(void);
void perCC112xSetOutputPower(uint8_t index);

void  cc112x_init(uint8_t freq, uint8_t pwr);
void RxMode();

void TxMoe();
uint8_t CC1120_setFrequency(float centre);
void read_rssi(void);








#endif
