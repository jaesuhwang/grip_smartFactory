/*
 * sleep.c
 *
 *  Created on: 2022. 3. 23.
 *      Author: luckyman
 */

#include "global.h"
#include "sleep.h"
#include "rtc.h"
#include "main.h"
#include "sleep.h"
#include "sleep_init.h"
#include "tdma_n.h"

#define RTC_32768_TICK  0.00048828125f

uint8_t sleepMode;
uint8_t sleep_init;

uint8_t sleep_flag;
uint8_t sleep_min;
uint8_t sleep_sec;


void gpioAllAnalogSet()
{
	uint16_t tempPort;
	uint8_t rc;
	GPIO_InitTypeDef GPIO_InitStructure;
#if 0


	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	//HAL_ADC_MspDeInit(&hadc2);

	HAL_NVIC_DisableIRQ(EXTI2_IRQn);

	HAL_GPIO_WritePin(UHF_INT_PORT, UHF_INT_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EX_3V_PORT, EX_3V_PIN | EX_5V_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(USER_UART_GET_IN_PORT, USER_UART_GET_IN_PIN, GPIO_PIN_RESET);

	HAL_Delay(100);

	GPIO_InitStructure.Pin = EX_3V_PIN|EX_5V_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(EX_5V_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = UHF_INT_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(UHF_INT_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = USER_UART_GET_IN_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(USER_UART_GET_IN_PORT, &GPIO_InitStructure);

	CC1120_SpiCmdStrobe(CC112X_SIDLE);
	do
	{
	  rc = CC1120_SpiCmdStrobe(CC112X_SNOP);
	//__no_operation();
	} while ((rc & 0xF0) != 0x00);

	CC1120_SpiCmdStrobe(CC112X_SPWD);

	HAL_SPI_MspDeInit(&hspi1);

	tempPort = GPIO_PIN_All;
	tempPort &= ~( EX_3V_PIN| EX_5V_PIN | UART_RX_PIN);
	GPIO_InitStructure.Pin = tempPort;
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

	tempPort = GPIO_PIN_All;
	//tempPort &= ~( GPIO_PIN_2);
	GPIO_InitStructure.Pin = tempPort;
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = GPIO_PIN_All;
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
	HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);

	__HAL_RCC_GPIOA_CLK_DISABLE();
	__HAL_RCC_GPIOB_CLK_DISABLE();
	__HAL_RCC_GPIOC_CLK_DISABLE();
	__HAL_RCC_GPIOD_CLK_DISABLE();
	__HAL_RCC_GPIOE_CLK_DISABLE();
#endif

//#define LED1_PORT				GPIOC
//#define LED1_PIN				GPIO_PIN_10

//	GPIO_InitStructure.Pin = GPIO_PIN_All;
//	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
//	GPIO_InitStructure.Pull = GPIO_NOPULL;
//	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
//
//	__HAL_RCC_GPIOC_CLK_DISABLE();

//	GPIO_InitStructure.Pin = GPIO_PIN_8;
//	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
//	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
//	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

}


void SYSCLKConfig_STOP(void)  // 자는거 
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  uint32_t pFLatency = 0;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* Get the Oscillators configuration according to the internal RCC registers */
  HAL_RCC_GetOscConfig(&RCC_OscInitStruct);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_NONE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /* Get the Clocks configuration according to the internal RCC registers */
  HAL_RCC_GetClockConfig(&RCC_ClkInitStruct, &pFLatency);

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType     = RCC_CLOCKTYPE_SYSCLK;
  RCC_ClkInitStruct.SYSCLKSource  = RCC_SYSCLKSOURCE_PLLCLK;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, pFLatency) != HAL_OK)
  {
    Error_Handler();
  }
}




void SleepModeEntryFunction(uint8_t min, uint8_t sec)  // sleep
{
	uint16_t timeSec;
	//HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);  // disable PA0
	//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
//	uint16_t sleepTime;
//	if ( sec  <= 0 )
//		return ;
	if ( min == 0 && sec ==0)
		return ;

	timeSec = (min * 60) + sec;
	//timeSec = 60 + sec;


	/** Now enter the standby mode **/
	


		/* Enable PWR clock enable */
		//__HAL_RCC_PWR_CLK_ENABLE();

		HAL_PWREx_DisableLowPowerRunMode();

		/* Suspend tick interrupt */
	    HAL_SuspendTick();

		/* Clear the WU FLAG */
		//__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
		__HAL_RTC_WAKEUPTIMER_EXTI_CLEAR_FLAG();
		/* clear the RTC Wake UP (WU) flag */
		__HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);

		HAL_SuspendTick();
		/*## Configure the Wake up timer ###########################################*/
		/*  RTC Wake-up Interrupt Generation:
	  	  Wake-up Time Base = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSI))
	  	  ==> WakeUpCounter = Wake-up Time / Wake-up Time Base

	  	  To configure the wake up timer to 20s the WakeUpCounter is set to 0xA017:
			RTC_WAKEUPCLOCK_RTCCLK_DIV = RTCCLK_Div16 = 16
			Wake-up Time Base = 16 /(32KHz) = 0.0005 seconds
			==> WakeUpCounter = ~10s/0.0005s = 20000 = 0x4E20 */

		/** Deactivate the RTC wakeup  **/

		//sleepTime = (uint16_t)((float)sec / RTC_32768_TICK);
		/* Finally enter the standby mode */
		if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, timeSec/*0x0FA0 */, RTC_WAKEUPCLOCK_CK_SPRE_16BITS) != HAL_OK)
		{
		  Error_Handler();
		}
		//HAL_PWR_EnterSTANDBYMode();
		//HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
		// HAL_SuspendTick(); // 슬립모드로 들어가기전 tick 중단해야할 수 도 있다

		//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET); // LED OFF

		//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);  // 3v volt enable

		// 1)
		gpioAllAnalogSet();
		sleep_deinit();
		// method 2)
		#ifdef SLEEP_MODE_MAIN


		HAL_SuspendTick();
		HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
		//HAL_PWR_EnterSTANDBYMode();
		#endif


		// method 2)
#if 1
		#ifdef STOP2_MODE

		//	LpClock_Config();
		//	HAL_LPTIM_Counter_Stop(&hlptim1);
		//	HAL_LPTIM_Counter_Start(&hlptim1,0xFFFF);
		HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
		//HAL_PWREx_EnterSTOP1Mode(PWR_STOPENTRY_WFI);
		#endif
#endif

}






void SleepWakeUpEntryFunction()
{
  /*** Check if the SB flag is set ***/

	

  if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)   // rtc 에서 wake up 했을 구문 빠짐
  {
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);  // clear the flag

	/** Blink the LED **/

	/** Disable the WWAKEUP PIN **/
	HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);  // disable PA0

	/** Deactivate the RTC wakeup  **/
	HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);

	SystemClock_Config();
	//SysTick_Config
	HAL_ResumeTick();

	//test = 1;
  }
}



uint8_t initialize;

uint32_t m_LpTimerCnt;

void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc) // wake up
{

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	uint16_t tempPort;
	char bvuf[128] = {0x00,};
	


	if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)   // rtc 에서 wake up 했을 구문 빠짐
	{
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);  // clear the flag
	}


	/** Disable the WWAKEUP PIN **/
	HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);  // disable PA0

	HAL_RTCEx_DeactivateWakeUpTimer(hrtc);

	SystemClock_Config();

	HAL_ResumeTick();

	sleep_init =1; // test
	
	//sleep_init();

	//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);

	//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET); // LED ON
	//mac_init(0);


#if 0
	m_LpTimerCnt = HAL_LPTIM_ReadCounter(&hlptim1);
	sprintf(bvuf, "ReadWakeUpTimCnt %d \r\n",m_LpTimerCnt );
	HAL_LPTIM_Counter_Stop(&hlptim1);
	HAL_UART_Transmit(&huart1, bvuf, strlen(bvuf), 100);

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();



	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

	GPIO_InitStruct.Pin = GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);


	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);

	GPIO_InitStruct.Pin = GPIO_PIN_12;  // USB PWR
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1 | GPIO_PIN_2, GPIO_PIN_SET);

	GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOA, CC1120_NSS_PIN| CC1120_MISO_PIN| CC1120_MOSI_PIN | CC1120_SCK_PIN, GPIO_PIN_RESET);

	tempPort = ( CC1120_NSS_PIN| CC1120_MISO_PIN| CC1120_MOSI_PIN | CC1120_SCK_PIN );
	GPIO_InitStruct.Pin = tempPort;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


#endif
	initialize = 0;

}
