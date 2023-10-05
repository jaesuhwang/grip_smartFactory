#include "bsp_adc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
// #include "adc.h"
#include "usart.h"
#include "running_avg.h"
#include "ac_voltage.h"
#include "global.h"
// #include "cli.h"

#define ADC_BUF_MAX 10

#define F_DRV_REQ  0x0F
#define F_DRV_BUSY 0xF0

#define ADC_RUNNING_INIT 0
#define ADC_RUNNING_WAIT 1
#define ADC_RUNNING_RUN  2



/* dma init - > adc -> time 6  ->  clear it  -> start dma-> timbase start it  */

extern ADC_HandleTypeDef hadc2;
extern DMA_HandleTypeDef hdma_adc2;

uint16_t adc_val[ADC_CH_NUM];   // 3ch ADC 
uint16_t avg_adc[ADC_CH_NUM];


#if 0
typedef struct 
{
  uint8_t sen_1_h,
  uint8_t sen_1_l
}sensor_data_t;


sensor_data_t sensor_data[3];
#endif


uint8_t sen_1_h;
uint8_t sen_1_l;
uint8_t sen_2_h;
uint8_t sen_2_l;
uint8_t sen_3_h;
uint8_t sen_3_l;



uint32_t adc_total;
float adc_tot_avg;
uint32_t p_out_tempbuf[ADC_BUF_MAX];
uint32_t h_vol_tempbuf[ADC_BUF_MAX];
uint32_t s_cul_tempbuf[ADC_BUF_MAX];

uint32_t adc_total_value[3];

uint32_t p_out_value;
uint32_t h_vol_value;       
uint32_t s_cul_value;
uint16_t sen_val;


void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{
  #if 1 
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(adcHandle->Instance==ADC2)
  {
  /* USER CODE BEGIN ADC2_MspInit 0 */

  /* USER CODE END ADC2_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_SYSCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* ADC2 clock enable */
    __HAL_RCC_ADC_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**ADC2 GPIO Configuration
    PA0     ------> ADC2_IN5
    PA1     ------> ADC2_IN6
    PA2     ------> ADC2_IN7
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* ADC2 DMA Init */
    /* ADC2 Init */
    hdma_adc2.Instance = DMA1_Channel2;
    hdma_adc2.Init.Request = DMA_REQUEST_0;
    hdma_adc2.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc2.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc2.Init.MemInc = DMA_MINC_ENABLE;
    
    if(sen_id == PHASE_2_VOL_ID || sen_id == PHASE_3_VOL_ID )
    {
      hdma_adc2.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
      hdma_adc2.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    }
    else 
    {
      hdma_adc2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
      hdma_adc2.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    }
    hdma_adc2.Init.Mode = DMA_CIRCULAR;
    hdma_adc2.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_adc2) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(adcHandle,DMA_Handle,hdma_adc2);

  /* USER CODE BEGIN ADC2_MspInit 1 */

  /* USER CODE END ADC2_MspInit 1 */
  }
  #endif 
  #if 0
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(adcHandle->Instance==ADC2)
  {
  /* USER CODE BEGIN ADC2_MspInit 0 */

  /* USER CODE END ADC2_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_SYSCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* ADC2 clock enable */
    __HAL_RCC_ADC_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**ADC2 GPIO Configuration
    PA0     ------> ADC2_IN5
    PA1     ------> ADC2_IN6
    PA2     ------> ADC2_IN7
    PA3     ------> ADC2_IN8
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* ADC2 DMA Init */
    /* ADC2 Init */
    hdma_adc2.Instance = DMA1_Channel2;
    hdma_adc2.Init.Request = DMA_REQUEST_0;
    hdma_adc2.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc2.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc2.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc2.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc2.Init.Mode = DMA_CIRCULAR;
    hdma_adc2.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_adc2) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(adcHandle,DMA_Handle,hdma_adc2);
  
  
  }
  #endif 
}




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


#if 0 
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    adc_conCplt = 1;

    // if(sen_id == NTC_SEN_ID && sen_id == NTC_SEN_ID)
    //sen_val = adc_val[0];
}

#endif 

