/*
 * pinDef.h
 *
 *  Created on: 2022. 3. 23.
 *      Author: luckyman
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include "stm32l4xx_hal.h"




extern long ch2hex(char *hex, uint8_t size);
extern void hex2ch(uint8_t hex, char *chBuff);
extern void power_init();
//extern config_data configs;
//int read_configuration_data(config_data *c);


#define NTC_SEN_ID                0x01
#define PHASE_3_VOL_ID            0x02
#define PHASE_2_VOL_ID            0x03
#define PREASURE_ID               0x04
#define WATER_LEAK_ID             0x05
#define VIB_ID                    0x06
#define UL_SONIC_ID               0x07
#define PHASE_3_CUR_ID            0x08
#define PHASE_2_CUR_ID            0x09  









#define ADC2_IN6_PORT			GPIOA
#define ADC2_IN6_PIN			GPIO_PIN_0
#define ADC2_IN7_PORT			GPIOA
#define ADC2_IN7_PIN			GPIO_PIN_1
#define ADC2_IN14_PORT			GPIOA
#define ADC2_IN14_PIN			GPIO_PIN_2
#define ADC2_IN15_PORT			GPIOA
#define ADC2_IN15_PIN			GPIO_PIN_3

#define EN_3V3_PORT				GPIOB
#define EN_3V3_PIN				GPIO_PIN_4
#define EN_5V0_PORT				GPIOC
#define EN_5V0_PIN				GPIO_PIN_8

#define LED1_PORT				GPIOC
#define LED1_PIN				GPIO_PIN_10
#define LED2_PORT				GPIOC
#define LED2_PIN				GPIO_PIN_11

#define KEY_IN1_PORT			GPIOC
#define KEY_IN1_PIN				GPIO_PIN_13

#define GPIO0_PORT				GPIOA
#define GPIO0_PIN				GPIO_PIN_8
#define GPIO1_PORT				GPIOA
#define GPIO1_PIN				GPIO_PIN_11
#define GPIO2_PORT				GPIOA
#define GPIO2_PIN				GPIO_PIN_12
#define GPIO3_PORT				GPIOC
#define GPIO3_PIN				GPIO_PIN_0
#define GPIO4_PORT				GPIOC
#define GPIO4_PIN				GPIO_PIN_1
#define GPIO5_PORT				GPIOC
#define GPIO5_PIN				GPIO_PIN_7

#define GPIO_COM_ID0_PORT		GPIOC
#define GPIO_COM_ID0_PIN		GPIO_PIN_6

#define SPI1_NSS_PORT			GPIOA
#define SPI1_NSS_PIN			GPIO_PIN_4
#define SPI1_SCK_PORT			GPIOA
#define SPI1_SCK_PIN			GPIO_PIN_5
#define SPI1_MISO_PORT			GPIOA
#define SPI1_MISO_PIN			GPIO_PIN_6
#define SPI1_MOSI_PORT			GPIOA
#define SPI1_MOSI_PIN			GPIO_PIN_7

#define UART1_TX_PORT			GPIOA
#define UART1_TX_PIN			GPIO_PIN_9
#define UART1_RX_PORT			GPIOA
#define UART1_RX_PIN			GPIO_PIN_10

#define DBG_LED_PORT    		GPIOA
#define DBG_LED_PIN     		GPIO_PIN_15

#define SPI2_NSS_PORT			GPIOB
#define SPI2_NSS_PIN			GPIO_PIN_8
#define SPI2_NSS1_PORT			GPIOB
#define SPI2_NSS1_PIN			GPIO_PIN_5
#define SPI2_NSS2_PORT			GPIOB
#define SPI2_NSS2_PIN			GPIO_PIN_9
#define SPI2_SCK_PORT			GPIOB
#define SPI2_SCK_PIN			GPIO_PIN_10
#define SPI2_MISO_PORT			GPIOC
#define SPI2_MISO_PIN			GPIO_PIN_2
#define SPI2_MOSI_PORT			GPIOC
#define SPI2_MOSI_PIN			GPIO_PIN_3

#define UART3_TX_PORT			GPIOC
#define UART3_TX_PIN			GPIO_PIN_4
#define UART3_RX_PORT			GPIOC
#define UART3_RX_PIN			GPIO_PIN_5

#define UHF_RST_PORT			GPIOC
#define UHF_RST_PIN				GPIO_PIN_9

#define I2C_SDA_PORT			GPIOB
#define I2C_SDA_PIN				GPIO_PIN_11
#define I2C_SCL_PORT			GPIOB
#define I2C_SCL_PIN				GPIO_PIN_13
#define I2C_A0_PORT				GPIOB
#define I2C_A0_PIN				GPIO_PIN_14
#define I2C_A1_PORT				GPIOB
#define I2C_A1_PIN				GPIO_PIN_15

#define SEN_ID0_PORT			GPIOB
#define SEN_ID0_PIN				GPIO_PIN_0
#define SEN_ID1_PORT			GPIOB
#define SEN_ID1_PIN				GPIO_PIN_1
#define SEN_ID2_PORT			GPIOB
#define SEN_ID2_PIN				GPIO_PIN_2
#define SEN_ID3_PORT			GPIOB
#define SEN_ID3_PIN				GPIO_PIN_3

#define UART5_TX_PORT			GPIOC
#define UART5_TX_PIN			GPIO_PIN_12
#define UART5_RX_PORT			GPIOD
#define UART5_RX_PIN			GPIO_PIN_2

extern uint8_t sen_id;
extern void read_sen_id();

#endif /* GLOBAL_H_ */

