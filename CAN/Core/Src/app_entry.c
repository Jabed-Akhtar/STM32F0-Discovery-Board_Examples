/*
 * app_entry.c
 *
 *  Created on: May 16, 2025
 *      Author: jabed
 */
#include "main.h"
#include "stm32f0xx_hal.h"


extern TIM_HandleTypeDef htim6;

void app_entry(void)
{
  HAL_TIM_Base_Start_IT(&htim6);

  while(1)
  {
	//HAL_Delay(1000);
	//HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9);

	__WFI();
  }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);

  // Toggle the LEDs
  if (htim->Instance == TIM6){
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9);
  }
}
