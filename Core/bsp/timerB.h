/*
 * timerB.h
 *
 *  Created on: 2022. 2. 9.
 *      Author: lucky
 */

#ifndef INC_TIMERB_H_
#define INC_TIMERB_H_


#include "main.h"

#define TIMERB_CCR_NUMBER 7



#define TIMERB_ALARM_CCR0 0
#define TIMERB_ALARM_CCR1 1
#define TIMERB_ALARM_CCR2 2
#define TIMERB_ALARM_CCR3 3
#define TIMERB_ALARM_CCR4 4
#define TIMERB_ALARM_CCR5 5
#define TIMERB_ALARM_CCR6 6
#define TIMERB_ALARM_OVER 7

typedef uint16_t (*timerBcb)(void);


void timerB_init();
uint16_t timerB_register_cb (uint16_t alarm, timerBcb f);
uint16_t timerB_time();
void timerB_stop();
uint16_t timerB_set_alarm_from_now  (uint16_t alarm, uint16_t ticks, uint16_t period);
uint16_t timerB_set_alarm_from_time (uint16_t alarm, uint16_t ticks, uint16_t period, uint16_t ref);
uint16_t timerB_unset_alarm(uint16_t alarm);

#endif /* INC_TIMERB_H_ */
