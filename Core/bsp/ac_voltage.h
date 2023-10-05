

#include "main.h"
#if 0

extern void phase_2_volt();

#endif

extern uint8_t adc_complete;
extern uint32_t adc_ac_val[1400];
extern uint16_t adc_Count;
extern void ac_voltage_deinit();

extern uint8_t tempBuffer[200];
extern void ac_dma_start();
extern void AC_Volt_adc_init(void);
extern double    Vrms_r,Vrms_s,Vrms_t;


extern void phase_2_volt();
