/*
 * clock.c
 *
 *  Created on: 2022. 1. 31.
 *      Author: lucky
 */

#include "arm-def.h"
#include <stdio.h>
#include "main.h"

volatile unsigned long seconds;
volatile clock_time_t ticks;


void clock_init(void)
{
	ticks = 0;
	seconds = 0;
}

unsigned long clock_seconds(void)
{
  return seconds;
}

void clock_set_seconds(unsigned long sec)
{
  seconds = sec;
}

clock_time_t clock_time(void)
{
  return ticks;
}

void
clock_delay_usec(uint16_t usec)
{
  HAL_Delay((uint32_t) usec/ ((uint32_t) 1000));
}

void clock_wait(clock_time_t t)
{
  clock_time_t start;
  start = clock_time();
  while((clock_time() - start) < (clock_time_t)t ){
  }
}

