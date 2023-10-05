
#ifndef RUNNING_AVERAGE_H_
#define RUNNING_AVERAGE_H_

#include "running_avg.h"
#include "stm32l4xx_hal.h"

#define SENSOR_CNT_MAXSIZE	3


#define RUNNING_SIZE_DEPTH				100

typedef struct
{
	uint8_t _size;
	uint8_t _cnt;
	uint8_t _idx;
	float _sum;	
	float _min;
	float _max;
	float _ar[RUNNING_SIZE_DEPTH];	
} RunningAverage_t;


extern RunningAverage_t Running_average[SENSOR_CNT_MAXSIZE];
//extern RunningAverage_t Running_average[];


void RunningAverage_init(RunningAverage_t *ra );
void RunningAverage_clear(RunningAverage_t *ra );


void RunningAverage_addValue(RunningAverage_t *ra, float f);


// returns the average of the data-set added sofar
float RunningAverage_getAverage(RunningAverage_t *ra);


// returns the value of an element if exist, 0 otherwise
float RunningAverage_getElement(RunningAverage_t *ra, uint8_t idx);


// fill the average with a value
// the param number determines how often value is added (weight)
// number should preferably be between 1 and size
void RunningAverage_fillValue(RunningAverage_t *ra, float value, int number);


float RunningAverage_getMinInBuffer(RunningAverage_t *ra);
float RunningAverage_getMaxInBuffer(RunningAverage_t *ra);



#endif

