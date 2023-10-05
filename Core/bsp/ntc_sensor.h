#ifndef _NTC_SENSOR_H_
#define _NTC_SENSOR_H_

#include "main.h"


extern void get_temp();
extern void ntc_drv();
extern void ntc_init();
extern void ntc_deinit();

extern void ntc_dma_start();

#endif
