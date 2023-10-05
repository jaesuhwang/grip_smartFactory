/*
 * clock.h
 *
 *  Created on: 2022. 1. 31.
 *      Author: lucky
 */

#ifndef INC_CLOCK_H_
#define INC_CLOCK_H_

#include "contiki.h"



#ifdef CLOCK_CONF_SECOND
#define CLOCK_SECOND CLOCK_CONF_SECOND
#else
#define CLOCK_SECOND (clock_time_t)32
#endif

void clock_init(void);
clock_time_t clock_time(void);


unsigned long clock_seconds(void);

void clock_set_seconds(unsigned long sec);

void clock_wait(clock_time_t t);


void clock_delay_usec(uint16_t dt);

//int clock_fine_max(void);
//unsigned short clock_fine(void);
//void clock_delay(unsigned int delay);



#endif /* INC_CLOCK_H_ */
