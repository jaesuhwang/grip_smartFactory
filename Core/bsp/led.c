
#include "led.h"

void dbg_led_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();

  HAL_GPIO_WritePin(DBG_LED_PORT, DBG_LED_PIN, GPIO_PIN_SET);

  GPIO_InitStruct.Pin = DBG_LED_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(DBG_LED_PORT, &GPIO_InitStruct);
}

void dbg_led_on(void)
{
    HAL_GPIO_WritePin(DBG_LED_PORT, DBG_LED_PIN, GPIO_PIN_RESET);
}

void dbg_led_off(void)
{
    HAL_GPIO_WritePin(DBG_LED_PORT, DBG_LED_PIN, GPIO_PIN_SET);
}

void dbg_led_toggle(void)
{
    HAL_GPIO_TogglePin(DBG_LED_PORT, DBG_LED_PIN);
}

void led1_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();

	HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, GPIO_PIN_RESET);

	GPIO_InitStruct.Pin = LED1_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(LED1_PORT, &GPIO_InitStruct);
}

void led1_on(void)
{
    HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, GPIO_PIN_SET);
}

void led1_off(void)
{
    HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, GPIO_PIN_RESET);
}

void led2_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOC_CLK_ENABLE();

	HAL_GPIO_WritePin(LED2_PORT, LED2_PIN, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA15 */
  GPIO_InitStruct.Pin = LED2_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(LED2_PORT, &GPIO_InitStruct);
}

void led2_on(void)
{
    HAL_GPIO_WritePin(LED2_PORT, LED2_PIN, GPIO_PIN_SET);
}

void led2_off(void)
{
    HAL_GPIO_WritePin(LED2_PORT, LED2_PIN, GPIO_PIN_RESET);
}
