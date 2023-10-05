
#include "ac_current.h"
#include "bsp_adc.h"
#include "sensor_drv.h"
#include "adc.h"

#define PHASE_2_CUR_MAX_SIZE		22
#define PHASE_3_CUR_MAX_SIZE		22


extern ADC_HandleTypeDef hadc2;
extern DMA_HandleTypeDef hdma_adc2;
uint16_t ac_cur_val[4];


typedef struct
{
	float phase_2_cur;
} phase_2_cur_t;

typedef struct
{
	float phase_2_adc;
} phase_2_adc_t;

typedef struct
{
	float phase_3_cur;
} phase_3_cur_t;

typedef struct
{
	float phase_3_adc;
} phase_3_adc_t;



phase_2_cur_t phase_2_cur[PHASE_2_CUR_MAX_SIZE] = {
		0,
		0.1,
		0.2,
		0.3,
		0.4,
		0.5,
		0.6,
		0.7,
		0.8,
		0.9,
		1,
		1.1,
		1.2,
		1.3,
		1.4,
		1.5,
		3.8,
		5,
		6.7,
		8,
		9.5,
		13
};

phase_2_adc_t phase_2_adc[PHASE_2_CUR_MAX_SIZE] = {
	240,
	247,
	258,
	283,
	325,
	368,
	417,
	465,
	510,
	561,
	615,
	665,
	730,
	795,
	850,
	970,
	2500,
	2650,
	2850,
	2900,
	2910,
	3030
};





phase_3_cur_t phase_3_cur[PHASE_3_CUR_MAX_SIZE] = {
	0,
	0.1,
	0.2,
	0.3,
	0.4,
	0.5,
	0.6,
	0.7,
	0.8,
	0.9,
	1,
	1.1,
	1.2,
	1.3,
	1.4,
	1.5,
	3.8,
	5,
	6.7,
	8,
	9.5,
	13
};

phase_3_adc_t phase_3_adc[PHASE_3_CUR_MAX_SIZE] = {
	240,
	247,
	258,
	283,
	325,
	368,
	417,
	465,
	510,
	561,
	615,
	665,
	730,
	795,
	850,
	970,
	2500,
	2650,
	2850,
	2900,
	2910,
	3030

};

#if 1 

void phase_cur_dma_start()
{
  HAL_ADC_Start_DMA(&hadc2,(uint32_t*)ac_cur_val, 4);
}
void phase_cur_init()
{
  
	/* DMA init */
	__HAL_RCC_DMA1_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA1_Channel2_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 5, 1);
	HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);

	/* ADC init */
	  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */

  /** Common config
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV32;
  hadc2.Init.Resolution = ADC_RESOLUTION_12B;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc2.Init.LowPowerAutoWait = DISABLE;
  hadc2.Init.ContinuousConvMode = ENABLE;
  hadc2.Init.NbrOfConversion = 4;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc2.Init.DMAContinuousRequests = ENABLE;
  hadc2.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc2.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_12CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = ADC_REGULAR_RANK_4;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */



}

void phase_cur_init_deinit()
{
  HAL_ADC_Stop_DMA(&hadc2);
  __HAL_RCC_DMA1_CLK_DISABLE();
  HAL_ADC_DeInit(&hadc2);
  HAL_NVIC_DisableIRQ(DMA1_Channel2_IRQn);
  __HAL_RCC_ADC_CLK_DISABLE();
  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
  HAL_DMA_DeInit(hadc2.DMA_Handle);
}


#endif

float phase_map(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float phase_multiMap(float val, float* _in, float* _out, uint16_t size){
    // take care the value is within range
    // val = constrain(val, _in[0], _in[size-1]);
    if (val <= _in[0]) return _out[0];
    if (val >= _in[size-1]) return _out[size-1];

    // search right interval
    uint8_t pos = 1; // _in[0] allready tested
    while(val > _in[pos]) pos++;

    // this will handle all exact “points” in the _in array
    if (val == _in[pos]) return _out[pos];

    // interpolate in the right segment for the rest
    return phase_map(val, _in[pos-1], _in[pos], _out[pos-1], _out[pos]);
}


void get_2phase_cur_val()
{
	uint16_t phase_2_cur_val;
  float temp_sen;

  temp_sen = phase_multiMap((float)ac_cur_val[0], &phase_2_adc, &phase_2_cur, PHASE_2_CUR_MAX_SIZE);

#if 1 
	phase_2_cur_val = (uint16_t)(temp_sen*10);
#endif
  //phase_2_cur_val = 1234;
  sen_1_h = phase_2_cur_val / 100; // 백의자리 십의자리
  sen_1_l = phase_2_cur_val % 100; //일의자리 소수첫째자리

}
float temp_value[3];

void get_3phase_cur_val()
{
	uint16_t phase_3_R_val;
  uint16_t phase_3_S_val;
  uint16_t phase_3_T_val;

  temp_value[0] = phase_multiMap((float)ac_cur_val[0], &phase_3_adc, &phase_3_cur, PHASE_3_CUR_MAX_SIZE);
  temp_value[1] = phase_multiMap((float)ac_cur_val[1], &phase_3_adc, &phase_3_cur, PHASE_3_CUR_MAX_SIZE);
  temp_value[2] = phase_multiMap((float)ac_cur_val[2], &phase_3_adc, &phase_3_cur, PHASE_3_CUR_MAX_SIZE);
	

  phase_3_R_val = (uint16_t)(temp_value[0]*10);
  phase_3_S_val = (uint16_t)(temp_value[1]*10);
  phase_3_T_val = (uint16_t)(temp_value[2]*10);

#if 0
	phase_3_R_val = 1234;
	phase_3_S_val = 4567;
	phase_3_T_val = 7700;
#endif
	sen_1_h = phase_3_R_val/100; // 백의자리 십의자리
	sen_1_l = phase_3_R_val%100; //일의자리 소수첫째자리
	sen_2_h = phase_3_S_val/100; // 백의자리 십의자리   
	sen_2_l = phase_3_S_val%100; // 십의자리 소수첫째자리 
	sen_3_h = phase_3_T_val/100; // 백의자리 십의자리 
	sen_3_l = phase_3_T_val%100; // 십의자리 소수첫째자리
}


void ac_current_deinit()
{
  HAL_ADC_Stop_DMA(&hadc2);
  __HAL_RCC_DMA1_CLK_DISABLE();
  HAL_ADC_DeInit(&hadc2);
  HAL_NVIC_DisableIRQ(DMA1_Channel2_IRQn);
  __HAL_RCC_ADC_CLK_DISABLE();
  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
  HAL_DMA_DeInit(hadc2.DMA_Handle);
}
