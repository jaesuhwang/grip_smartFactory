#include "main.h"
#include "spi.h"
#include "usart.h"
#include "sensor_drv.h"
#include "cc1120.h"
#include "sleep_init.h"
#include "tdma_n.h"
extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim6;
extern UART_HandleTypeDef huart1;
extern ADC_HandleTypeDef hadc2;

void sleep_deinit()
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	uint16_t temp_pin;
#if 1  // cc1120 deinit
    uint8_t rc;
    CC1120_SpiCmdStrobe(CC112X_SIDLE);
    do
    {
        rc = CC1120_SpiCmdStrobe(CC112X_SNOP);
    //__no_operation();
    } while ((rc & 0xF0) != 0x00);

    CC1120_SpiCmdStrobe(CC112X_SPWD);

#endif

#if 0
    sensor_deinit();

    /* ADC */
    HAL_ADC_Stop_DMA(&hadc2);
    __HAL_RCC_DMA1_CLK_DISABLE();
    HAL_ADC_DeInit(&hadc2);
    HAL_NVIC_DisableIRQ(DMA1_Channel2_IRQn);
    __HAL_RCC_ADC_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    HAL_DMA_DeInit(hadc2.DMA_Handle);


   /* AC_V*/
   HAL_NVIC_DisableIRQ(TIM6_IRQn); //
   HAL_TIM_OC_DeInit(&htim6);
   HAL_TIM_Base_Stop_IT(&htim6);     
   HAL_TIM_Base_DeInit(&htim6);
    __HAL_RCC_TIM6_CLK_DISABLE();


   
   /*TIM 15*/
   __HAL_RCC_TIM15_CLK_DISABLE();

#endif
   //HAL_SuspendTick();
   /* GPIO */
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_8);
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_12);
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_14);


    HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
    HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);

   /*TIM 4*/ // USE CC1120
   __HAL_TIM_DISABLE_IT(&htim4, TIM_IT_CC1);
   __HAL_TIM_DISABLE_IT(&htim4, TIM_IT_CC2);
   __HAL_TIM_DISABLE_IT(&htim4, TIM_IT_CC3);

    HAL_TIM_OC_Stop_IT(&htim4, TIM_CHANNEL_1);
    HAL_TIM_OC_Stop_IT(&htim4, TIM_CHANNEL_2);
    HAL_TIM_OC_Stop_IT(&htim4, TIM_CHANNEL_3);
    HAL_TIM_Base_Stop(&htim4);


    HAL_NVIC_DisableIRQ(TIM4_IRQn);
    HAL_TIM_OC_DeInit(&htim4);
    HAL_TIM_Base_DeInit(&htim4);
    __HAL_RCC_TIM4_CLK_DISABLE();

   /* UART1*/
   HAL_GPIO_DeInit(GPIOA,GPIO_PIN_9);
   HAL_GPIO_DeInit(GPIOA,GPIO_PIN_10);
   HAL_NVIC_DisableIRQ(USART1_IRQn);
   __HAL_RCC_USART1_CLK_DISABLE();
   HAL_UART_DeInit(&huart1);
    

    /* UART3*/
   HAL_GPIO_DeInit(GPIOA,GPIO_PIN_4);
   HAL_GPIO_DeInit(GPIOA,GPIO_PIN_5);
   HAL_NVIC_DisableIRQ(USART3_IRQn);
   __HAL_RCC_USART3_CLK_DISABLE();
   HAL_UART_DeInit(&huart3);

   


   /* SPI */
    //HAL_SPI_MspDeInit(&hspi1);
    HAL_GPIO_DeInit(GPIOA,GPIO_PIN_5);
    HAL_GPIO_DeInit(GPIOA,GPIO_PIN_6);
    HAL_GPIO_DeInit(GPIOA,GPIO_PIN_7);
    HAL_SPI_DeInit(&hspi1);
    __HAL_RCC_SPI1_CLK_DISABLE();
    HAL_SPI_MspDeInit(&hspi1);


   /* GPIO */
   #if 0
    HAL_GPIO_DeInit(GPIOA,GPIO_PIN_4);
    HAL_GPIO_DeInit(GPIOA,GPIO_PIN_8);
    HAL_GPIO_DeInit(GPIOA,GPIO_PIN_12);
    HAL_GPIO_DeInit(GPIOA,GPIO_PIN_15);

    HAL_GPIO_DeInit(GPIOB,GPIO_PIN_0);
    HAL_GPIO_DeInit(GPIOB,GPIO_PIN_1);
    HAL_GPIO_DeInit(GPIOB,GPIO_PIN_2);
    HAL_GPIO_DeInit(GPIOB,GPIO_PIN_3);
    HAL_GPIO_DeInit(GPIOB,GPIO_PIN_8);
    HAL_GPIO_DeInit(GPIOB,GPIO_PIN_13);

    HAL_GPIO_DeInit(GPIOC,GPIO_PIN_9);
    HAL_GPIO_DeInit(GPIOC,GPIO_PIN_10);
    HAL_GPIO_DeInit(GPIOC,GPIO_PIN_11);
#endif 
   #if 1
    temp_pin = GPIO_PIN_4 | GPIO_PIN_8 | GPIO_PIN_12 | GPIO_PIN_15 ;
    //temp_pin = GPIO_PIN_4 | GPIO_PIN_8 | GPIO_PIN_12 | GPIO_PIN_15;
    GPIO_InitStructure.Pin = temp_pin;
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

	temp_pin = GPIO_PIN_All;
	temp_pin &= ~GPIO_PIN_8 ; // 3v
	GPIO_InitStructure.Pin = temp_pin;
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);


	temp_pin = GPIO_PIN_All;
	temp_pin &= ~GPIO_PIN_8 ; // 5v
	GPIO_InitStructure.Pin = temp_pin;
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);  // 3v volt disable
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);  // 5v volt disable

    #endif 

    __HAL_RCC_GPIOA_CLK_DISABLE();
    __HAL_RCC_GPIOB_CLK_DISABLE();
    __HAL_RCC_GPIOC_CLK_DISABLE();
    __HAL_RCC_GPIOD_CLK_DISABLE();




    /* TSET */
    #if 0 
	temp_pin = GPIO_PIN_All;
	temp_pin &= ~GPIO_PIN_8 ; // 3v
	GPIO_InitStructure.Pin = temp_pin;
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

	temp_pin = GPIO_PIN_All;
	temp_pin &= ~GPIO_PIN_8 ;
	GPIO_InitStructure.Pin = temp_pin;
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = GPIO_PIN_All;
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
	HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);
    #endif
//	GPIO_InitStructure.Pin = GPIO_PIN_All;
//	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
//	GPIO_InitStructure.Pull = GPIO_NOPULL;
//	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
//	__HAL_RCC_GPIOC_CLK_DISABLE();
//	GPIO_InitStructure.Pin = GPIO_PIN_8;
//	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
//	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
//	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

}
void wakeup_init()
{
    //HAL_Init();
    MX_GPIO_Init();
    MX_USART1_UART_Init();
    //MX_SPI1_Init();
    MX_TIM4_Init();
    //MX_TIM15_Init();
	MX_USART3_UART_Init();


	power_init();

    // JKU
    HAL_Delay(500);  // 필요함
    
    // HJS
    MX_SPI1_Init();
    MX_RTC_Init(); 
    
    // JKU
    read_sen_id();
    
    //sensor_init();
    //uartRingBuff_init();
    //__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
    
    //mac_set_access_allowed_cb(mac_ready); // 수정
}
