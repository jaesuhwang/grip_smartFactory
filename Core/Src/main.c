/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "ac_voltage.h"
#include "appDef.h"

#include "led.h"
#include "pressur.h"
#include "spi_function.h"
#include "cc1120_ref_reg.h"
#include "cc112x_spi.h"
#include "sensor_drv.h"
#include "cc1120.h"
#include "tdma_n.h"
#include "sleep.h"
#include "global.h"
#include "ringbuf.h"
#include "packet.h"
#include "bsp_adc.h"
#include "bsp_mpu6050.h"
#include "urm37_distance.h"
#include "sleep_init.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void read_data(void);
/* USER CODE BEGIN PFP */
extern uint8_t tx_complete;

volatile uint16_t counter_test;
uint16_t mac_ready(void) {
	// return 1 to wake the cpu up
#if 0
	sleepMode = 0 ;
#endif
	return 1;
}

int _write(int file, char *p, int len) {
	HAL_UART_Transmit(&huart1, p, len, 10);
	return len;
}

extern uint8_t sentComplete;

extern ADC_HandleTypeDef hadc2;
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


uint64_t readFlashSector[256] = { 0, };
uint32_t writeFlashSector[512] = { 0, };

uint8_t DBG_LED;
uint8_t sensor_id;
uint8_t once;
uint8_t test_conut;
int sleep_test = 0;

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */
	uint32_t sysTick;
	uint8_t addr;
	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();
	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();

	MX_USART1_UART_Init();
	MX_SPI1_Init();
	MX_TIM4_Init();
	//MX_TIM15_Init(); //
	MX_USART3_UART_Init(); //
	power_init();   //
	HAL_Delay(300);
	MX_RTC_Init(); // 
	read_sen_id();
	/* USER CODE BEGIN 2 */

	//sensor_init();
	read_data(); // eep read data

	cc1120_freq_set(sensor_id);
	uartRingBuff_init();
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	
	/* USER CODE END 2 */

	//HAL_ADC_Start_DMA(&hadc2,(uint32_t*)test_value, 1);
	/* Infinite loop */
	/* USER CODE BEGIN WHILE */

	if (sen_id != 0) { 
		mac_init(1);
		mac_set_access_allowed_cb(mac_ready);
	}
	sysTick = HAL_GetTick();
	while (1) {
		/* USER CODE END WHILE */
		if (sleep_init == 1) {
			wakeup_init();
			read_data();
			mac_init(1);
			mac_set_access_allowed_cb(mac_ready);  // TEST ING
			sleep_init = 0;
		}
		/* USER CODE BEGIN 3 */
		//sensor_drv();
		uart2_receive_func();

#if 1
		if (mac_is_access_allowed()) {

			uint8_t bat_h = 5;
			uint8_t bat_l = 5;
			mac_payload[0] = sen_id;
			mac_payload[1] = sen_1_h;
			mac_payload[2] = sen_1_l;
			mac_payload[3] = sen_2_h;
			mac_payload[4] = sen_2_l;
			mac_payload[5] = sen_3_h;
			mac_payload[6] = sen_3_l;
			mac_payload[7] = bat_h;
			mac_payload[8] = bat_l;
			mac_send();
		}
#endif
	}
	/* USER CODE END 3 */
}

void read_data(void) {
	read_configuration_data(&configs);
	//node_addr = configs.NID;
	node_addr = 2;
	sensor_id = configs.sensorID;
	g_channel = configs.ch;
	txPowerTeb = configs.pw;  // Tx power
	DBG_LED = configs.DBG_led;
	sleep_flag = configs.sleepOn;  // on 1 off 0
}
/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1)
			!= HAL_OK) {
		Error_Handler();
	}

	/** Configure LSE Drive Capability
	 */
	HAL_PWR_EnableBkUpAccess();
	__HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI
			| RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
