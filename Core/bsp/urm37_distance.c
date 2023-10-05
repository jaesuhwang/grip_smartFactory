#include "urm37_distance.h"
#include "tim.h"
#include "sensor_drv.h"
#include "bsp_adc.h"


extern TIM_HandleTypeDef htim15;

uint8_t 	ultra_busy;
uint8_t 	ultraStartIs = 0 ;
float 		ultra_sen_value;
uint8_t 	test_dly;
uint32_t 	urm37_delay;
void count_reset()
{
	
	TIM15->CNT = 0;

}



void ultraTimerStop()
{
	uint32_t distance_value;
	ultra_sen_value= (TIM15->CNT)/50;
	HAL_TIM_Base_Stop(&htim15);
	// HAL_TIM_Base_Stop_IT(&htim15);
	distance_value = (uint32_t)(ultra_sen_value *10);
	sen_1_h = distance_value/1000;    
	sen_1_l = distance_value%1000/100;   
	sen_2_h = distance_value%1000%100/10;
	sen_2_l = distance_value%1000%100%10;
}




void ultraStart(void)
{

	if( HAL_GetTick() - urm37_delay > 500)
	{
		ultra_busy = 0;
	}

	if(!ultra_busy)
	{
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_14);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
		HAL_Delay(1);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
		count_reset();
		urm37_delay = HAL_GetTick();
		ultra_busy = 1;
	}
}

  
void ultraExtPin_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
	GPIO_InitStruct.Pin = GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);


	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);  
  
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_14);

}


