/**
  ******************************************************************************
  * @file           : app_entry.c
  * @brief          : ...
  * @author			: jabed
  * @created		: June 14, 2025
  ******************************************************************************
  * -
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f0xx_hal.h"

#include <stdio.h>


/* Private define ------------------------------------------------------------*/
#define PWM_DUTY_PERC_MIN 70
#define PWM_DUTY_PREC_MAX 90

/* Private variables ---------------------------------------------------------*/
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern DMA_HandleTypeDef hdma_tim2_ch1;
extern TIM_HandleTypeDef htim6;
extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_tx;

uint8_t uart_tx_pwm[20] = {'\0'};
uint8_t uart_tx_app_status[20] = {'\0'};

uint8_t pwm_duty_percentage;

uint32_t IC_Val1 = 0;
uint32_t IC_Val2 = 0;
uint32_t Difference = 0;
int Is_First_Captured = 0;

/* Measure Frequency */
float frequency = 0;


/**
  * @brief  The application entry point.
  * @retval int
  */
void app_entry(void) {
	/* ***** Inits ***** */
	uint8_t msg_init[] = "\n\rProgram started.";
	HAL_UART_Transmit(&huart1, msg_init, sizeof(msg_init), HAL_MAX_DELAY);

	HAL_TIM_Base_Start_IT(&htim6);

	pwm_duty_percentage = PWM_DUTY_PERC_MIN;
	TIM2->CCR1 = pwm_duty_percentage; // 50% duty cycle
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);

	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);

	while(1) {
		__WFI();
	}
}


/**
  * @brief  EXTI line Interrupt Handler (for e.g. on pressing Push-Button on-board)
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	pwm_duty_percentage = pwm_duty_percentage + 10;
	if (pwm_duty_percentage>PWM_DUTY_PREC_MAX)
	{
		pwm_duty_percentage = PWM_DUTY_PERC_MIN;
	}
	TIM2->CCR1 = pwm_duty_percentage;

	sprintf((char *)uart_tx_pwm, "\n\tPWM Perc.: %d", pwm_duty_percentage);
	HAL_UART_Transmit(&huart1, uart_tx_pwm, sizeof(uart_tx_pwm), HAL_MAX_DELAY);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
	if (htim->Instance == TIM6) {
		HAL_GPIO_TogglePin(GPIOC, LED_Blue_Pin);

		sprintf((char *)uart_tx_app_status, "\n\rDiff.: %lu", Difference);
		HAL_UART_Transmit(&huart1, uart_tx_app_status, sizeof(uart_tx_app_status), HAL_MAX_DELAY);
	}
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM3){
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			if (Is_First_Captured==0) // if the first rising edge is not captured
			{
				IC_Val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read the first value
				Is_First_Captured = 1;  // set the first captured as true
			}
			else   // If the first rising edge is captured, now we will capture the second edge
			{
				IC_Val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);  // read second value
				if (IC_Val2 > IC_Val1)
				{
					Difference = IC_Val2-IC_Val1;
				}
				else if (IC_Val1 > IC_Val2)
				{
					Difference = (0xffff - IC_Val1) + IC_Val2;	// 32-bit -> 0xffff
				}

				HAL_GPIO_TogglePin(GPIOC, LED_Green_Pin);

//				sprintf(tx_data_dma, "\n\rDiff.: %d", Difference);
//				HAL_UART_Transmit_DMA(&huart1, tx_data_dma, sizeof(tx_data_dma));


//				float refClock = TIMCLOCK/(PRESCALAR);
//				frequency = refClock/Difference;
//				__HAL_TIM_SET_COUNTER(htim, 0);  // reset the counter
				Is_First_Captured = 0; // set it back to false
			}
		}
	}
}
