

#if 1

#include "ac_voltage.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "bsp_adc.h"

#define adc_bit 4095
#define ref_v 3.3
#define nuber_of_sample 300

extern UART_HandleTypeDef huart1;
extern ADC_HandleTypeDef hadc2;
extern TIM_HandleTypeDef htim6;


extern DMA_HandleTypeDef hdma_adc2;


uint32_t adc_ac_val[1400];
uint16_t adc_Count;
uint8_t adc_complete;
uint32_t adc_temp_r[400];
uint32_t adc_temp_s[400];
uint32_t adc_temp_t[400];
uint32_t zero_point_r;
uint32_t zero_point_s;
uint32_t zero_point_t;

int32_t   Vnom_r,Vnom_s,Vnom_t;
uint32_t  Vsum_r,Vsum_s,Vsum_t;
double    Vrms_r,Vrms_s,Vrms_t;


uint8_t tempBuffer[200];


int  r,s,t = 0;

void phase_2_volt()
{
	uint16_t temp_vrms;

    if ( adc_complete == 1 )
	  {
		  for ( int i = 0 ; i < nuber_of_sample*3 ; i++)
		  {
			  if ( i % 3 == 0 ) //ch 5
			  {
				  adc_temp_r[r] = adc_ac_val[i];
				  r++;
			  }
		  }

		  for (int i = 0; i < nuber_of_sample; i++)
		  {
			for (int j = 0; j < (nuber_of_sample - 1) - i; j++)
			{
				if (adc_temp_r[j] > adc_temp_r[j + 1])
				{	// 버블 정렬 사용
					uint32_t temp;
					temp = adc_temp_r[j];
					adc_temp_r[j] = adc_temp_r[j + 1];
					adc_temp_r[j + 1] = temp;
				}
			}
		  }
		  zero_point_r = (adc_temp_r[0]+adc_temp_r[nuber_of_sample-1])/2;

//		  sprintf(tempBuffer, "--------nom %d sum %d \r\n",adc_temp[0], adc_temp[99] );
//		  HAL_UART_Transmit(&huart1, tempBuffer, strlen(tempBuffer) , 100);


		  for ( int i = 0 ; i < nuber_of_sample*3 ; i++)
		  {
			  if ( i % 3 == 0 ) //ch 5
			  {
				  Vnom_r = adc_ac_val[i] - zero_point_r;
				  Vsum_r += Vnom_r*Vnom_r;
				  //sprintf(tempBuffer, "nom %d sum %d \r\n",Vnom_r,Vsum_r );
				  //HAL_UART_Transmit(&huart1, tempBuffer, strlen(tempBuffer) , 100);
			  }
		  }
		  Vrms_r = sqrt(Vsum_r/nuber_of_sample)/adc_bit*ref_v/0.0020;
		  //sprintf(tempBuffer, "Vrms_r = %.2f \r\n", Vrms_r );
		  //HAL_UART_Transmit(&huart1, tempBuffer, strlen(tempBuffer) , 100);
		  if(Vrms_r >3.0)
		  {
			temp_vrms = Vrms_r * 10;
			sen_1_h = temp_vrms / 100; // 백의자리 십의자리
			sen_1_l = temp_vrms % 100; //일의자리 소수첫째자리
		  }
		  else
		  {
			temp_vrms = 0;
			sen_1_h = temp_vrms / 100; // 백의자리 십의자리
			sen_1_l = temp_vrms % 100; //일의자리 소수첫째자리
		  }

			r = 0;
			s = 0;
			t = 0;
			adc_complete = 0;
			zero_point_r = 0;
			zero_point_s = 0;
			zero_point_t = 0;

			Vnom_r = 0;
			Vnom_s = 0;
			Vnom_t = 0;
			Vsum_r = 0;
			Vsum_s = 0;
			Vsum_t = 0;
			adc_complete =0;
	  }
}


void phase_3_volt()
{
	uint16_t temp_vrms;

    if ( adc_complete == 1 )
	  {
		  for ( int i = 0 ; i < nuber_of_sample*3 ; i++)
		  {
			  if ( i % 3 == 0 ) //R
			  {
				  adc_temp_r[r] = adc_ac_val[i];
				  r++;
			  }
              if ( i % 3 == 1 ) //S
			  {
				  adc_temp_s[s] = adc_ac_val[i];
				  s++;
			  }
              if ( i % 3 == 2 ) //T
			  {
				  adc_temp_t[t] = adc_ac_val[i];
				  t++;
			  }
		  }

		  for (int i = 0; i < nuber_of_sample; i++)
		  {
			for (int j = 0; j < (nuber_of_sample - 1) - i; j++)
			{
				if (adc_temp_r[j] > adc_temp_r[j + 1])
				{	// 버블 정렬 사용
					uint32_t temp;
					temp = adc_temp_r[j];
					adc_temp_r[j] = adc_temp_r[j + 1];
					adc_temp_r[j + 1] = temp;
				}
                if (adc_temp_s[j] > adc_temp_s[j + 1])
				{	// 버블 정렬 사용
					uint32_t temp;
					temp = adc_temp_s[j];
					adc_temp_s[j] = adc_temp_s[j + 1];
					adc_temp_s[j + 1] = temp;
				}
                if (adc_temp_t[j] > adc_temp_t[j + 1])
				{	// 버블 정렬 사용
					uint32_t temp;
					temp = adc_temp_t[j];
					adc_temp_t[j] = adc_temp_t[j + 1];
					adc_temp_t[j + 1] = temp;
				}
			}
		  }
		  zero_point_r = (adc_temp_r[0]+adc_temp_r[nuber_of_sample-1])/2;
          zero_point_s = (adc_temp_s[0]+adc_temp_s[nuber_of_sample-1])/2;
          zero_point_t = (adc_temp_t[0]+adc_temp_t[nuber_of_sample-1])/2;

		  //sprintf(tempBuffer, "--------nom %d sum %d \r\n",adc_temp_s[0], adc_temp_s[99] );
		  //HAL_UART_Transmit(&huart1, tempBuffer, strlen(tempBuffer) , 100);


		  for ( int i = 0 ; i < nuber_of_sample*3 ; i++)
		  {
			  if ( i % 3 == 0 ) //ch 5
			  {
				  Vnom_r = adc_ac_val[i] - zero_point_r;
				  Vsum_r += Vnom_r*Vnom_r;
				  //sprintf(tempBuffer, "nom %d sum %d \r\n",Vnom,Vsum );
				  //HAL_UART_Transmit(&huart1, tempBuffer, strlen(tempBuffer) , 100);
			  }
			  else if ( i % 3 == 1)
			  {
                  Vnom_s = adc_ac_val[i] - zero_point_s;
				  Vsum_s += Vnom_s*Vnom_s;
				  //sprintf(tempBuffer, "ch6 %d\r\n",adc_val[i] );
				  //HAL_UART_Transmit(&huart1, tempBuffer, strlen(tempBuffer) , 100);
			  }
			  else if ( i % 3 == 2 )
			  {
                  Vnom_t = adc_ac_val[i] - zero_point_t;
				  Vsum_t += Vnom_t*Vnom_t;
				  ///sprintf(tempBuffer, "ch7 %d\r\n",adc_val[i] );
				  //HAL_UART_Transmit(&huart1, tempBuffer, strlen(tempBuffer) , 100);
			  }
		  }
		  Vrms_r = sqrt(Vsum_r/nuber_of_sample)/adc_bit*ref_v/0.00315;
		  //sprintf(tempBuffer, "Vrms_r %.2f \r\n", Vrms_r );
		  //HAL_UART_Transmit(&huart1, tempBuffer, strlen(tempBuffer) , 100);
          Vrms_s = sqrt(Vsum_s/nuber_of_sample)/adc_bit*ref_v/0.00315;
		  //sprintf(tempBuffer, "Vrms_s %.2f \r\n", Vrms_s );
		  //HAL_UART_Transmit(&huart1, tempBuffer, strlen(tempBuffer) , 100);
          Vrms_t = sqrt(Vsum_t/nuber_of_sample)/adc_bit*ref_v/0.00315;
		  //sprintf(tempBuffer, "Vrms_t %.2f \r\n", Vrms_t );
		  //HAL_UART_Transmit(&huart1, tempBuffer, strlen(tempBuffer) , 100);

		  
			if(Vrms_r >3.0)
			{
				temp_vrms = Vrms_r * 10;
				sen_1_h = temp_vrms / 100; // 백의자리 십의자리
				sen_1_l = temp_vrms % 100; //일의자리 소수첫째자리

				temp_vrms = Vrms_s * 10;
				sen_2_h = temp_vrms / 100; // 백의자리 십의자리
				sen_2_l = temp_vrms % 100; //일의자리 소수첫째자리

				temp_vrms = Vrms_t * 10;
				sen_3_h = temp_vrms / 100; // 백의자리 십의자리
				sen_3_l = temp_vrms % 100; //일의자리 소수첫째자리
			}
			else
			{
				temp_vrms = 0;
				sen_1_h = temp_vrms / 100; // 백의자리 십의자리
				sen_1_l = temp_vrms % 100; //일의자리 소수첫째자리

				temp_vrms = 0;
				sen_2_h = temp_vrms / 100; // 백의자리 십의자리
				sen_2_l = temp_vrms % 100; //일의자리 소수첫째자리

				temp_vrms = 0;
				sen_3_h = temp_vrms / 100; // 백의자리 십의자리
				sen_3_l = temp_vrms % 100; //일의자리 소수첫째자리
			}

			
			r = 0;
			s = 0;
			t = 0;
			adc_complete = 0;
			zero_point_r = 0;
			zero_point_s = 0;
			zero_point_t = 0;

			Vnom_r = 0;
			Vnom_s = 0;
			Vnom_t = 0;
			Vsum_r = 0;
			Vsum_s = 0;
			Vsum_t = 0;

			adc_complete =0;
	}
}



/*
타이머 콜백 
*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)  
{
  if(htim->Instance == TIM6)
  {
	  if ( adc_Count++ >=  nuber_of_sample )
	  {
		HAL_ADC_Stop_DMA(&hadc2);
		HAL_TIM_Base_Stop_IT(&htim6);
		adc_complete = 1;
		adc_Count = 0;
	  }
  }
}


void ac_dma_start()
{
	HAL_ADC_Start_DMA(&hadc2,(uint32_t*)adc_ac_val, 1500);
	HAL_TIM_Base_Start_IT(&htim6);
}


 void ac_voltage_deinit()
 {
	HAL_ADC_Stop_DMA(&hadc2);
	__HAL_RCC_DMA1_CLK_DISABLE();
	HAL_ADC_DeInit(&hadc2);
	HAL_NVIC_DisableIRQ(DMA1_Channel2_IRQn);
	__HAL_RCC_ADC_CLK_DISABLE();
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
	HAL_DMA_DeInit(hadc2.DMA_Handle);
 }
 

/* ADC2 init function */
void AC_Volt_adc_init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */

  /** Common config
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc2.Init.Resolution = ADC_RESOLUTION_12B;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc2.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  hadc2.Init.LowPowerAutoWait = DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.NbrOfConversion = 3;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T6_TRGO;
  hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
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
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
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
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

}



#endif
