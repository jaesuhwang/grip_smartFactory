#include "pressur.h"
#include "bsp_adc.h"
#include "sensor_drv.h"
#include "adc.h"

#define PRESSURE_MAX_SIZE		25

extern ADC_HandleTypeDef hadc2;
extern DMA_HandleTypeDef hdma_adc2;
uint16_t press_val[1];
float   outValue; 

typedef struct
{
	float PressMPa;
} Pressure_MPa_t;

typedef struct
{
	float PressAdc;
} Pressure_Adc_t;


Pressure_MPa_t pressMpa[PRESSURE_MAX_SIZE] = {
		0,
		0.05,
		0.1,
		0.15,
		0.2,
		0.25,
		0.3,
		0.35,
		0.4,
		0.45,
		0.5,
		0.55,
		0.6,
		0.65,
		0.7,
		0.75,
		0.8,
		0.85,
		0.9,
		0.95,
		1,
		1.05,
		1.1,
		1.15,
		1.2
};

Pressure_Adc_t pressAdc[PRESSURE_MAX_SIZE] = {
		400,
		470,
		600,
		750,
		900,
		1030,
		1170,
		1300,
		1440,
		1560,
		1690,
		1820,
		1950,
		2110,
		2250,
		2380,
		2512,
		2647.5,
		2783,
		2918.5,
		3054,
		3189.5,
		3325,
		3460.5,
		3596
};


void pressur_init()
{
  
  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */

  /** Common config
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV16;
  hadc2.Init.Resolution = ADC_RESOLUTION_12B;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc2.Init.LowPowerAutoWait = DISABLE;
  hadc2.Init.ContinuousConvMode = ENABLE;
  hadc2.Init.NbrOfConversion = 1;
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
  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_47CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */


}



void pressur_dma_start()
{
  HAL_ADC_Start_DMA(&hadc2,(uint32_t*)press_val, 1);
}

void pressur_deinit()
{
  HAL_ADC_Stop_DMA(&hadc2);
  __HAL_RCC_DMA1_CLK_DISABLE();
  HAL_ADC_DeInit(&hadc2);
  HAL_NVIC_DisableIRQ(DMA1_Channel2_IRQn);
  __HAL_RCC_ADC_CLK_DISABLE();
  
  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
  HAL_DMA_DeInit(hadc2.DMA_Handle);
}


float map(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


float multiMap(float val, float* _in, float* _out, uint16_t size){
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
    return map(val, _in[pos-1], _in[pos], _out[pos-1], _out[pos]);
}


void get_press_val()
{
	uint16_t press;
	outValue = multiMap(press_val[0], &pressAdc, &pressMpa, PRESSURE_MAX_SIZE);
	press = (uint16_t)(outValue*100);
	//press = 1234;
	sen_1_h = press/100; // 십의자리 일의자리
	sen_1_l = press%100; // 소수첫째 소수둘째


}
