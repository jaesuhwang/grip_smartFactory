/*
 * bsp_mpu6050.h
 *
 *  Created on: 2022. 4. 8.
 *      Author: luckyman
 */

#ifndef BSP_MPU6050_H_
#define BSP_MPU6050_H_

#include "main.h"



//=========================================================================

#define MPU6050_DataRate_8KHz       0   /*!< Sample rate set to 8 kHz */
#define MPU6050_DataRate_4KHz       1   /*!< Sample rate set to 4 kHz */
#define MPU6050_DataRate_2KHz       3   /*!< Sample rate set to 2 kHz */
#define MPU6050_DataRate_1KHz       7   /*!< Sample rate set to 1 kHz */
#define MPU6050_DataRate_500Hz      15  /*!< Sample rate set to 500 Hz */
#define MPU6050_DataRate_250Hz      31  /*!< Sample rate set to 250 Hz */
#define MPU6050_DataRate_125Hz      63  /*!< Sample rate set to 125 Hz */
#define MPU6050_DataRate_100Hz      79  /*!< Sample rate set to 100 Hz */

/* Acce sensitivities in g/s */
#define MPU6050_ACCE_SENS_2			((float) 16384)
#define MPU6050_ACCE_SENS_4			((float) 8192)
#define MPU6050_ACCE_SENS_8			((float) 4096)
#define MPU6050_ACCE_SENS_16		((float) 2048)

typedef enum MPU6050_Accelerometer_t {
	MPU6050_Accelerometer_2G = 0x00, /*!< Range is +- 2G */
	MPU6050_Accelerometer_4G = 0x01, /*!< Range is +- 4G */
	MPU6050_Accelerometer_8G = 0x02, /*!< Range is +- 8G */
	MPU6050_Accelerometer_16G = 0x03 /*!< Range is +- 16G */
} MPU6050_Accelerometer_t;


typedef struct _MPU6050_t {
	/* Private */
	uint8_t Address;         /*!< I2C address of device. Only for private use */
	float Gyro_Mult;         /*!< Gyroscope corrector from raw data to "degrees/s". Only for private use */
	float Acce_Mult;         /*!< Accelerometer corrector from raw data to "g". Only for private use */
	/* Public */
//	int16_t Accelerometer_X; /*!< Accelerometer value X axis */
//	int16_t Accelerometer_Y; /*!< Accelerometer value Y axis */
//	int16_t Accelerometer_Z; /*!< Accelerometer value Z axis */
	float Accelerometer_X; /*!< Accelerometer value X axis */
	float Accelerometer_Y; /*!< Accelerometer value Y axis */
	float Accelerometer_Z; /*!< Accelerometer value Z axis */
	int16_t Gyroscope_X;     /*!< Gyroscope value X axis */
	int16_t Gyroscope_Y;     /*!< Gyroscope value Y axis */
	int16_t Gyroscope_Z;     /*!< Gyroscope value Z axis */
	float Temperature;       /*!< Temperature in degrees */
} MPU6050_t;

typedef enum _MPU6050_Gyroscope_t {
	MPU6050_Gyroscope_250s = 0x00,  /*!< Range is +- 250 degrees/s */
	MPU6050_Gyroscope_500s = 0x01,  /*!< Range is +- 500 degrees/s */
	MPU6050_Gyroscope_1000s = 0x02, /*!< Range is +- 1000 degrees/s */
	MPU6050_Gyroscope_2000s = 0x03  /*!< Range is +- 2000 degrees/s */
} MPU6050_Gyroscope_t;

/* Gyro sensitivities in degrees/s */
#define MPU6050_GYRO_SENS_250		((float) 131)
#define MPU6050_GYRO_SENS_500		((float) 65.5)
#define MPU6050_GYRO_SENS_1000		((float) 32.8)
#define MPU6050_GYRO_SENS_2000		((float) 16.4)


extern I2C_HandleTypeDef hi2c2;


////////////////////////////////////////////////////////////////////////
extern void Bsp_mpu_6050_Init(MPU6050_t *mpuChip , uint8_t ad0);
extern uint8_t Bsp_mpu_6050_WHO_AM_I(MPU6050_t *mpuChip);
extern void Bsp_mpu_6050_wake_up(MPU6050_t *mpuChip);
extern void Bsp_mpu_6050_set_sample_rate(MPU6050_t *mpuChip, uint8_t rate);
extern void Bsp_mpu_6050_SetAccelerometer(MPU6050_t *mpuChip,MPU6050_Accelerometer_t AccelerometerSensitivity);
extern void TM_MPU6050_ReadAccelerometer(MPU6050_t* mpuChip);
extern void TM_MPU6050_ReadGyroscope(MPU6050_t* mpuChip);


extern MPU6050_t mpu6050;




#endif /* BSP_MPU6050_H_ */
