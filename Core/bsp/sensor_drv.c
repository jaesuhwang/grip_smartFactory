
#include "sensor_drv.h"
#include "main.h"
#include "global.h"
#include "bsp_mpu6050.h"
#include "dma.h"
#include "bsp_adc.h"
#include "ntc_sensor.h"
#include "ac_current.h"
#include "ac_voltage.h"


extern TIM_HandleTypeDef htim15;
extern TIM_HandleTypeDef htim6;
extern ADC_HandleTypeDef hadc2;
extern I2C_HandleTypeDef hi2c2;
extern MPU6050_t mpu6050;



/*
#define NTC_SEN_ID                0x00  + 1 =1
#define PHASE_3_VOL_ID            0x01  + 1 =2
#define PHASE_2_VOL_ID            0x02  + 1 =3
#define PREASURE_ID               0x03  + 1 =4
#define WATER_LEAK_ID             0x04  + 1 =5
#define VIB_ID                    0x05  + 1 =6
#define UL_SONIC_ID               0x06  + 1 =7
#define PHASE_3_CUR_ID            0x07  + 1 =8
#define PHASE_2_CUR_ID            0x08  + 1 =9

*/
 


void sensor_init()
{
    switch (sen_id)
    {
        case NTC_SEN_ID:
        {
            MX_DMA_Init();
            ntc_init();
            //HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);
            ntc_dma_start();
        }
        break;
        case PHASE_3_VOL_ID:
        {
            MX_DMA_Init();
            AC_Volt_adc_init();
            MX_TIM6_Init();
             HAL_Delay(500);
            __HAL_TIM_CLEAR_IT(&htim6, TIM_IT_UPDATE);
            HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);
            ac_dma_start();
            HAL_TIM_Base_Start_IT(&htim6);
        }
        break;
        case PHASE_2_VOL_ID:
        {
            MX_DMA_Init();
            AC_Volt_adc_init();
            MX_TIM6_Init();
             HAL_Delay(500);
            __HAL_TIM_CLEAR_IT(&htim6, TIM_IT_UPDATE);
            HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);
            ac_dma_start();
            HAL_TIM_Base_Start_IT(&htim6);
        }
        break;
        case PREASURE_ID:
        {
            
            MX_DMA_Init();
            pressur_init();
            pressur_dma_start();
        }
        break;
        case WATER_LEAK_ID:
        {
            MX_DMA_Init();
            water_det_init();
            water_dma_start();
        }
        break;
        case VIB_ID:
        {
            Bsp_mpu_6050_Init(&mpu6050 , 0);
            Bsp_mpu_6050_WHO_AM_I(&mpu6050);
            Bsp_mpu_6050_wake_up(&mpu6050);
            Bsp_mpu_6050_set_sample_rate(&mpu6050, MPU6050_DataRate_8KHz);
            Bsp_mpu_6050_SetAccelerometer(&mpu6050, MPU6050_Accelerometer_8G);
        }
        break;
        case UL_SONIC_ID:
        {
            ultraExtPin_init();
            __HAL_GPIO_EXTI_CLEAR_FLAG(EXTI15_10_IRQn);
        }
        break;
        case PHASE_3_CUR_ID:
        {
        	MX_DMA_Init();
			phase_cur_init();
			phase_cur_dma_start();
        }
        break;
        case PHASE_2_CUR_ID:
        {
        	MX_DMA_Init();
			phase_cur_init();
			phase_cur_dma_start();
        }
        break;
    }
}

void sensor_deinit()
{

    switch (sen_id)
    {
        case NTC_SEN_ID:
        {
            ntc_deinit();
        }
        break;
        case PHASE_3_VOL_ID:
        {
            ac_voltage_deinit();
        }
        break;
        case PHASE_2_VOL_ID:
        {
            ac_voltage_deinit();
        }
        break;
        case PREASURE_ID:
        {
            pressur_deinit();
        }
        break;
        case WATER_LEAK_ID:
        {
            water_det_deinit();
         }   
        break;
        case VIB_ID:
        {
           
        }
        break;
        case UL_SONIC_ID:
        {
            
        }
        break;
        case PHASE_3_CUR_ID:
        {
            ac_current_deinit();
        }
        break;
        case PHASE_2_CUR_ID:
        {
            ac_current_deinit();
        }
        break;
    }
}



void sensor_drv()
{
    
    switch (sen_id)
    {
        case NTC_SEN_ID:
        {
            ntc_drv();
        }
        break;
        case PHASE_3_VOL_ID:
        {
           if ( adc_Count == 0 && adc_complete == 0)
            {
                ac_dma_start();
                HAL_TIM_Base_Start_IT(&htim6);
            }
            phase_3_volt();
        }
        break;
        case PHASE_2_VOL_ID:
        {
            if ( adc_Count == 0 && adc_complete == 0)
            {
                ac_dma_start();
                HAL_TIM_Base_Start_IT(&htim6);
            }
            phase_2_volt();
        }
        break;
        case PREASURE_ID:
        {
            get_press_val();
        }
        break;
        case WATER_LEAK_ID:
        {
            water_det_drv();
        }   
        break;
        case VIB_ID:
        {
            TM_MPU6050_ReadAccelerometer(&mpu6050);
        }
        break;
        case UL_SONIC_ID:
        {
            ultraStart();
        }
        break;
        case PHASE_3_CUR_ID:
        {
        	get_3phase_cur_val();
        }
        break;
        case PHASE_2_CUR_ID:
        {
        	get_2phase_cur_val();
        }
        break;
    }

}
