#ifndef _BSP_ADC_H_
#define _BSP_ADC_H_


#include "stm32l4xx_hal.h"
#include "main.h"

#define ADC_CH_NUM 1 // nuber of adc ch 

extern uint16_t sen_val;
extern uint16_t adc_val[ADC_CH_NUM];  // 3ch ADC 

extern void adc_drv();


extern uint8_t sen_1_h;
extern uint8_t sen_1_l;
extern uint8_t sen_2_h;
extern uint8_t sen_2_l;
extern uint8_t sen_3_h;     
extern uint8_t sen_3_l;





extern void Get_adc_value();

#endif
