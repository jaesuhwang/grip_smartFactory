
#include "running_avg.h"
#include <math.h>
RunningAverage_t Running_average[SENSOR_CNT_MAXSIZE];

void RunningAverage_init(RunningAverage_t *ra )
{
	ra->_size = RUNNING_SIZE_DEPTH;

	RunningAverage_clear(ra);
}

void RunningAverage_clear(RunningAverage_t *ra )
{
	int i;
	ra->_cnt = 0;
	ra->_idx = 0;
	ra->_sum = 0.0;
	ra->_min = NAN;
	ra->_max = NAN;
	for ( i = 0; i < ra->_size ; i++ )
		ra->_ar[i] = 0;
}

void RunningAverage_addValue(RunningAverage_t *ra, float f)
{
   
//	if ( ra->_idx == 0 && ra->_ar[0] == 0 )
//	{
//		return ;
//	}

	ra->_sum -= ra->_ar[ra->_idx];
  ra->_ar[ra->_idx] = f;
  ra->_sum += ra->_ar[ra->_idx];
  ra->_idx++;
  if (ra->_idx == ra->_size) ra->_idx = 0;  //1 faster than %

  // handle min max
  if (ra->_cnt == 0) ra->_min = ra->_max = f;
  else if (f < ra->_min) ra->_min = f;
  else if (f > ra->_max) ra->_max = f;

  if (ra->_cnt < ra->_size) ra->_cnt++;
}

// returns the average of the data-set added sofar
float RunningAverage_getAverage(RunningAverage_t *ra)
{
    if (ra->_cnt == 0) return NAN;
    return ra->_sum / ra->_cnt;
}

// returns the value of an element if exist, 0 otherwise
float RunningAverage_getElement(RunningAverage_t *ra, uint8_t idx)
{
    if (ra->_idx >= ra->_cnt ) return NAN;
    return ra->_ar[idx];
}

// fill the average with a value
// the param number determines how often value is added (weight)
// number should preferably be between 1 and size
void RunningAverage_fillValue(RunningAverage_t *ra, float value, int number)
{
    RunningAverage_clear(ra);
    for (int i = 0; i < number; i++)
    {
        RunningAverage_addValue(ra, value);
    }
}

// returns the minimum value in the buffer
float RunningAverage_getMinInBuffer(RunningAverage_t *ra)
{
	uint8_t i;
	
  if (ra->_cnt == 0) return NAN;

  float min = ra->_ar[0];
  for (i = 1; i < ra->_cnt; i++)
  {
    if (ra->_ar[i] < min) min = ra->_ar[i];
  }
  return min;
}

// returns the maximum value in the buffer
float RunningAverage_getMaxInBuffer(RunningAverage_t *ra)
{
	uint8_t i;
  if (ra->_cnt == 0) return NAN;

  float max = ra->_ar[0];
  for (i = 1; i < ra->_cnt; i++)
  {
    if (ra->_ar[i] > max) max = ra->_ar[i];
  }
  return max;
}
