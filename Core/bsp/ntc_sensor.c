
#include "math.h"
#include "main.h"
#include "bsp_adc.h"
#include "global.h"

#include "ntc_sensor.h"
extern ADC_HandleTypeDef hadc2;
extern DMA_HandleTypeDef hdma_adc2;

/*
https://github.com/e-tinkers/ntc-thermistor-with-arduino-and-esp32/blob/master/ntc_3950.ino#L302
*/
/* R1 resistance */
#define NTC_UP_R 10000.0f

/* constants of Steinhart-Hart equation */
#define A 1.123686451
#define B 2.359980489
#define C 0.7299134923

float Ntc_Tmp;
float Ntc_Tmp_Raw;
uint16_t Ntc_R;


int ThermistorPin;
double adcMax =4095;
double Vs = 3.0;

double R1 = 10000.0;   // voltage divider resistor value
double Beta = 3950.0;  // Beta value
double To = 298.15;    // Temperature in Kelvin for 25 degree Celsius
double Ro = 10000.0;   // Resistance of Thermistor at 25 degree Celsius

uint16_t ntc_adc_val[1];




void get_temp()
{
	// uint16_t adc_val;
	// adc_val = sen_val;
	// /* calc. ntc resistance */
	// Ntc_R = ((NTC_UP_R)/((4095/(float)adc_val) - 1));
	// /* temp */
	// float Ntc_Ln = log(Ntc_R);
	// /* calc. temperature */
	// Ntc_Tmp = (1.0/(A + B*Ntc_Ln + C*Ntc_Ln*Ntc_Ln*Ntc_Ln)) - 273.15;

}


#if 1

void ntc_init()
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
  sConfig.Channel = ADC_CHANNEL_5;
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

#endif 
 #if 0 
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC2)
  {
  /* USER CODE BEGIN ADC2_MspDeInit 0 */

  /* USER CODE END ADC2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC_CLK_DISABLE();

    /**ADC2 GPIO Configuration
    PA0     ------> ADC2_IN5
    PA1     ------> ADC2_IN6
    PA2     ------> ADC2_IN7
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2);

    /* ADC2 DMA DeInit */
    HAL_DMA_DeInit(adcHandle->DMA_Handle);
  /* USER CODE BEGIN ADC2_MspDeInit 1 */

  /* USER CODE END ADC2_MspDeInit 1 */
  }
}

#endif 


void ntc_deinit()
{
  HAL_ADC_Stop_DMA(&hadc2);
  __HAL_RCC_DMA1_CLK_DISABLE();
  HAL_ADC_DeInit(&hadc2);
  HAL_NVIC_DisableIRQ(DMA1_Channel2_IRQn);
  __HAL_RCC_ADC_CLK_DISABLE();
  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2);
  HAL_DMA_DeInit(hadc2.DMA_Handle);
}


void ntc_dma_start()
{
  HAL_ADC_Start_DMA(&hadc2,(uint32_t*)ntc_adc_val, 1);
}

void ntc_drv() 
{

  // adc_val[0]
	double Vout, Rt = 0;
	double T, Tc, Tf = 0;

	double adc = 0;

	adc = ntc_adc_val[0];
	//adc = ADC_LUT[(int)adc];

	Vout = adc * Vs/adcMax;
	Rt = R1 * Vout / (Vs - Vout);

	T = 1/(1/To + log(Rt/Ro)/Beta);    // Temperature in Kelvin
	Tc = T - 273.15 ;                   // Celsius
	Tf = Tc * 9 / 5 + 32;              // Fahrenheit


	Tc -= 10.5;

	if( Tc >0)
	{
		sen_1_h = 0 ;
	}
	else
	{
		Tc = Tc*-1;
		sen_1_h = 1;
	}
	sen_1_l = (uint8_t)Tc;
	
	


}
