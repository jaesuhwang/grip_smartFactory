 /* bsp_mpu6050.c
 *
 *  Created on: 2022. 4. 8.
 *      Author: luckyman
 */


/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   This file provides code for the configuration
  *          of the I2C instances.
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

#include "bsp_mpu6050.h"
#include "bsp_adc.h"
#include "sensor_drv.h"
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

I2C_HandleTypeDef hi2c2;
MPU6050_t mpu6050;
/* I2C2 init function */
void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x0060030F;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(i2cHandle->Instance==I2C2)
  {
  /* USER CODE BEGIN I2C2_MspInit 0 */

  /* USER CODE END I2C2_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C2;
    PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C2 GPIO Configuration
    PB11     ------> I2C2_SDA
    PB13     ------> I2C2_SCL
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2C2 clock enable */
    __HAL_RCC_I2C2_CLK_ENABLE();
  /* USER CODE BEGIN I2C2_MspInit 1 */

  /* USER CODE END I2C2_MspInit 1 */
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{

  if(i2cHandle->Instance==I2C2)
  {
  /* USER CODE BEGIN I2C2_MspDeInit 0 */

  /* USER CODE END I2C2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C2_CLK_DISABLE();

    /**I2C2 GPIO Configuration
    PB11     ------> I2C2_SDA
    PB13     ------> I2C2_SCL
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_11);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_13);

  /* USER CODE BEGIN I2C2_MspDeInit 1 */

  /* USER CODE END I2C2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */


/* Default I2C address */
#define MPU6050_I2C_ADDR			0xD0		// 0x68 <<1

/* Who I am register value */
#define MPU6050_I_AM				0x68

/* MPU6050 registers */
#define MPU6050_AUX_VDDIO			0x01
#define MPU6050_SMPLRT_DIV			0x19
#define MPU6050_CONFIG				0x1A
#define MPU6050_GYRO_CONFIG			0x1B
#define MPU6050_ACCEL_CONFIG		0x1C
#define MPU6050_MOTION_THRESH		0x1F
#define MPU6050_INT_PIN_CFG			0x37
#define MPU6050_INT_ENABLE			0x38
#define MPU6050_INT_STATUS			0x3A
#define MPU6050_ACCEL_XOUT_H		0x3B
#define MPU6050_ACCEL_XOUT_L		0x3C
#define MPU6050_ACCEL_YOUT_H		0x3D
#define MPU6050_ACCEL_YOUT_L		0x3E
#define MPU6050_ACCEL_ZOUT_H		0x3F
#define MPU6050_ACCEL_ZOUT_L		0x40
#define MPU6050_TEMP_OUT_H			0x41
#define MPU6050_TEMP_OUT_L			0x42
#define MPU6050_GYRO_XOUT_H			0x43
#define MPU6050_GYRO_XOUT_L			0x44
#define MPU6050_GYRO_YOUT_H			0x45
#define MPU6050_GYRO_YOUT_L			0x46
#define MPU6050_GYRO_ZOUT_H			0x47
#define MPU6050_GYRO_ZOUT_L			0x48
#define MPU6050_MOT_DETECT_STATUS	0x61
#define MPU6050_SIGNAL_PATH_RESET	0x68
#define MPU6050_MOT_DETECT_CTRL		0x69
#define MPU6050_USER_CTRL			0x6A
#define MPU6050_PWR_MGMT_1			0x6B
#define MPU6050_PWR_MGMT_2			0x6C
#define MPU6050_FIFO_COUNTH			0x72
#define MPU6050_FIFO_COUNTL			0x73
#define MPU6050_FIFO_R_W			0x74
#define MPU6050_WHO_AM_I			0x75



void Bsp_mpu_6050_Init(MPU6050_t *mpuChip , uint8_t ad0)
{
	uint8_t DeviceNumber = 0x02;

	hi2c2.Instance = I2C2;
	hi2c2.Init.Timing = 0x0060030F;
	hi2c2.Init.OwnAddress1 = 0;
	hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c2.Init.OwnAddress2 = 0;
	hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c2) != HAL_OK)
	{
		Error_Handler();
	}


	if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
	{
		Error_Handler();
	}


	if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 3) != HAL_OK)
	{
		Error_Handler();
	}



	if ( ad0 == 0 )
		mpuChip->Address = MPU6050_I2C_ADDR;
	else if (ad0 == 1)
		mpuChip->Address = MPU6050_I2C_ADDR | (uint8_t)DeviceNumber;

	MX_I2C2_Init();
}

uint8_t Bsp_mpu_6050_WHO_AM_I(MPU6050_t *mpuChip)
{
	uint8_t register_to_access = MPU6050_WHO_AM_I;
	uint8_t temp = 0;

	HAL_I2C_Master_Transmit(&hi2c2, mpuChip->Address, &register_to_access, 1, 1000);
	HAL_I2C_Master_Receive(&hi2c2, mpuChip->Address, &temp, 1, 1000);

	return temp;
}

void Bsp_mpu_6050_wake_up(MPU6050_t *mpuChip)
{
	uint8_t data_to_write[2];

	data_to_write[0] = MPU6050_PWR_MGMT_1;
	data_to_write[1] = 0x00;

	HAL_I2C_Master_Transmit(&hi2c2, mpuChip->Address, data_to_write, 2, 1000);
}

void Bsp_mpu_6050_set_sample_rate(MPU6050_t *mpuChip, uint8_t rate)
{
	uint8_t data_to_write[2];

	data_to_write[0] = MPU6050_SMPLRT_DIV;
	data_to_write[1] = rate;

	HAL_I2C_Master_Transmit(&hi2c2, mpuChip->Address, data_to_write, 2, 1000);
}

void Bsp_mpu_6050_SetAccelerometer(MPU6050_t *mpuChip,MPU6050_Accelerometer_t AccelerometerSensitivity)
{
	uint8_t reg_to_access = MPU6050_ACCEL_CONFIG;
	uint8_t temp = 0 ;
	uint8_t data_to_write[3];

	HAL_I2C_Master_Transmit(&hi2c2, mpuChip->Address, &reg_to_access, 1, 1000);
	HAL_I2C_Master_Receive(&hi2c2, mpuChip->Address, &temp, 1, 1000);

	data_to_write[0] = MPU6050_ACCEL_CONFIG; //접근할 레지스터(GYRO_CONFIG)의 주소
	temp = (temp & 0xE7) | (uint8_t)AccelerometerSensitivity << 3;
	data_to_write[1] = temp;
	HAL_I2C_Master_Transmit(&hi2c2, mpuChip->Address, data_to_write, 2, 1000);

	HAL_I2C_Master_Transmit(&hi2c2, mpuChip->Address, &reg_to_access, 1, 1000);
	HAL_I2C_Master_Receive(&hi2c2, mpuChip->Address, &temp, 1, 1000);

	/* Set sensitivities for multiplying gyro and accelerometer data */
	switch (AccelerometerSensitivity) {
		case MPU6050_Accelerometer_2G:
			mpuChip->Acce_Mult = (float)1 / MPU6050_ACCE_SENS_2;
			break;
		case MPU6050_Accelerometer_4G:
			mpuChip->Acce_Mult = (float)1 / MPU6050_ACCE_SENS_4;
			break;
		case MPU6050_Accelerometer_8G:
			mpuChip->Acce_Mult = (float)1 / MPU6050_ACCE_SENS_8;
			break;
		case MPU6050_Accelerometer_16G:
			mpuChip->Acce_Mult = (float)1 / MPU6050_ACCE_SENS_16;
		default:
			break;
	}

}

void Bsp_mpu_6050_SetGyroscope(MPU6050_t* mpuChip, MPU6050_Gyroscope_t GyroscopeSensitivity) {
	uint8_t temp;

	uint8_t data_to_write[3];
	HAL_I2C_Master_Transmit(&hi2c2, mpuChip->Address, MPU6050_GYRO_CONFIG, 1, 1000);
	HAL_I2C_Master_Receive(&hi2c2, mpuChip->Address, &temp, 1, 1000);

	data_to_write[0] = MPU6050_GYRO_CONFIG; //접근할 레지스터(GYRO_CONFIG)의 주소
	temp = (temp & 0xE7) | (uint8_t)GyroscopeSensitivity << 3;
	data_to_write[1] = temp;
	HAL_I2C_Master_Transmit(&hi2c2, mpuChip->Address, data_to_write, 2, 1000);

	HAL_I2C_Master_Transmit(&hi2c2, mpuChip->Address, MPU6050_GYRO_CONFIG, 1, 1000);
	HAL_I2C_Master_Receive(&hi2c2, mpuChip->Address, &temp, 1, 1000);

	switch (GyroscopeSensitivity) {
		case MPU6050_Gyroscope_250s:
			mpuChip->Gyro_Mult = (float)1 / MPU6050_GYRO_SENS_250;
			break;
		case MPU6050_Gyroscope_500s:
			mpuChip->Gyro_Mult = (float)1 / MPU6050_GYRO_SENS_500;
			break;
		case MPU6050_Gyroscope_1000s:
			mpuChip->Gyro_Mult = (float)1 / MPU6050_GYRO_SENS_1000;
			break;
		case MPU6050_Gyroscope_2000s:
			mpuChip->Gyro_Mult = (float)1 / MPU6050_GYRO_SENS_2000;
		default:
			break;
	}
}
float x_data_sen;
float y_data_sen;
float z_data_sen;


void TM_MPU6050_ReadAccelerometer(MPU6050_t* mpuChip) {
	uint8_t data[6];
	HAL_StatusTypeDef hal_ok;
	uint8_t reg_to_access = MPU6050_ACCEL_XOUT_H;
	int16_t accX,accY,accZ;
	/* Read accelerometer data */
	HAL_I2C_Master_Transmit(&hi2c2, mpuChip->Address, &reg_to_access, 1, 1000);
	hal_ok = HAL_I2C_Master_Receive(&hi2c2, mpuChip->Address, data, 6, 1000);

	//TM_I2C_ReadMulti(MPU6050_I2C, DataStruct->Address, MPU6050_ACCEL_XOUT_H, data, 6);
	




	float Mult = mpuChip->Acce_Mult;
	/* Format */
	accX = (int16_t)(data[0] << 8 | data[1]);
	accY = (int16_t)(data[2] << 8 | data[3]);
	accZ = (int16_t)(data[4] << 8 | data[5]);


	mpuChip->Accelerometer_X = accX*Mult;
	mpuChip->Accelerometer_Y = accY*Mult;
	mpuChip->Accelerometer_Z = accZ*Mult;


	x_data_sen = ((mpuChip->Accelerometer_X)*10);
	y_data_sen = ((mpuChip->Accelerometer_Y)*10);
	z_data_sen = ((mpuChip->Accelerometer_Z)*10);
	
	// sen_1_h = 0;
	// sen_2_h = 0;
	// sen_3_h = 0;
	
	if(x_data_sen < 0)
	{
		sen_1_h  = 1;
		x_data_sen =x_data_sen*-1; 
	}
	else
	{
		sen_1_h  = 0;
	}
	if(y_data_sen < 0)
	{
		sen_2_h  = 1;
		y_data_sen =y_data_sen*-1;
	}
	else
	{
		sen_2_h  = 0;
	}
	if(z_data_sen < 0)
	{
		sen_3_h  = 1;
		z_data_sen =z_data_sen*-1;
	}
	else
	{
		sen_3_h  = 0;
	}

	sen_1_l =  x_data_sen*10;
	sen_2_l =  y_data_sen*10;
	sen_3_l =  z_data_sen*10;

	// uint8_t sen_1_h;
	// uint8_t sen_1_l;
	// uint8_t sen_2_h;
	// uint8_t sen_2_l;
	// uint8_t sen_3_h;
	// uint8_t sen_3_l;


	/* Return OK */
//	return TM_MPU6050_Result_Ok;
}

void TM_MPU6050_ReadGyroscope(MPU6050_t* mpuChip) {

	/* Read gyroscope data */
	uint8_t data[6];

	HAL_I2C_Master_Transmit(&hi2c2, mpuChip->Address, MPU6050_GYRO_XOUT_H, 1, 1000);
	HAL_I2C_Master_Receive(&hi2c2, mpuChip->Address, data, 6, 1000);

	/* Format */
	mpuChip->Gyroscope_X = (int16_t)(data[0] << 8 | data[1]);
	mpuChip->Gyroscope_Y = (int16_t)(data[2] << 8 | data[3]);
	mpuChip->Gyroscope_Z = (int16_t)(data[4] << 8 | data[5]);
}


/* USER CODE END 1 */


