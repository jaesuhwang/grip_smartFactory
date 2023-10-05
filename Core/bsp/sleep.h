/*
 * sleep.h
 *
 *  Created on: 2022. 3. 23.
 *      Author: luckyman
 */

#ifndef SLEEP_H_
#define SLEEP_H_

#include "main.h"

#define STOP2_MODE



extern uint8_t sleep_init;

extern uint8_t sleep_flag;
extern uint8_t sleep_min;
extern uint8_t sleep_sec;
//void SleepModeEntryFunction();

void SleepModeEntryFunction(uint8_t min, uint8_t sec);


#endif /* SLEEP_H_ */
