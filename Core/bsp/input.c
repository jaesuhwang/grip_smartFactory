
#include "input.h"


#define SEN_ID0_PORT			GPIOB
#define SEN_ID0_PIN				GPIO_PIN_0
#define SEN_ID1_PORT			GPIOB
#define SEN_ID1_PIN				GPIO_PIN_1
#define SEN_ID2_PORT			GPIOB
#define SEN_ID2_PIN				GPIO_PIN_2
#define SEN_ID3_PORT			GPIOB
#define SEN_ID3_PIN				GPIO_PIN_3



uint16_t SensorInputKey(void)
{
    uint16_t det;

    det = SEN_ID0_PORT->IDR & ( SEN_ID0_PIN |
								SEN_ID1_PIN |
								SEN_ID2_PIN |
								SEN_ID3_PIN
                                );

    return det;
}

uint8_t key_In1(void)
{
	return KEY_IN1_PORT->IDR & KEY_IN1_PIN;
}
