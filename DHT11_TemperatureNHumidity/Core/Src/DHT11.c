/**
******************************************************************************
* @file           : DHT11.c
* @brief          : Driver for DHT11 Humidity and Temperature Sensor
******************************************************************************
* @author			: Jabed-Akhtar (GitHub)
* @date			: 2024.12.23
* @Licence		: MIT Licence
*******************************************************************************
* Descriptions:
*		- This file has functions to initialise, check the presence/response of the
*		  DHT11, and read values.
* How to use:
* 	1. Start the Sensor
* 		- Command: DHT11_Start()
* 	2. Verify the response
* 		- Command: uint8_t response = DHT11_Response();
*
* 		- 0: Error getting response from DHT11
* 		- 1: Successfully got response from DHT11
* 	3. Read Values
* 		- Command: uint8_t data = DHT11_GetData();
* 		- Read data one after another (No delay in between)
*
* 		3.1. Read Humidity integer value
* 		3.2. Read Humidity decimal value
* 		3.3. Read Temperature integer value
* 		3.4. Read Temperature decimal value
* 		3.5. Read Checksum
* 	4. Verify Data with checksum
* 		- Checksum must be equal to sum of all other four data values
* Example:
* 	DHT11_Start();
* 	uint8_t response 		= DHT11_Response();
* 	uint8_t Hum_byte_int 	= DHT11_GetData();
* 	uint8_t Hum_byte_dec 	= DHT11_GetData();
* 	uint8_t Temp_byte_int 	= DHT11_GetData();
* 	uint8_t Temp_byte_dec 	= DHT11_GetData();
* 	uint8_t checksum 		= DHT11_GetData();
* 	if (response==1 && checksum==Hum_byte_int+Hum_byte_dec+Temp_byte_int+Temp_byte_dec)
* 	{
* 		// Use Data / Send out Data
* 	}
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "DHT11.h"

/* Variables -----------------------------------------------------------------*/
extern TIM_HandleTypeDef htim14;

/* Functions -----------------------------------------------------------------*/
/*
 * Start the DHT11
 */
void DHT11_Start(void)
{
	/* Initialise the Data pin to Output mode */
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = DHT11_Out_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(DHT11_Out_GPIO_Port, &GPIO_InitStruct);

	/* Pull the Data line Low for 18 ms */
	HAL_GPIO_WritePin(DHT11_Out_GPIO_Port, DHT11_Out_Pin, GPIO_PIN_RESET);
	Delay_Ms(18);

	/* Release the Data line (Pull-up) and set it as input */
	HAL_GPIO_WritePin(DHT11_Out_GPIO_Port, DHT11_Out_Pin, GPIO_PIN_SET);
	Delay_Us(10);	// !!! Not waiting here is causing error reading data
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(DHT11_Out_GPIO_Port, &GPIO_InitStruct);
}

/*
 * Check the response of the DHT11
 * After initialisation, the data line must go low for about 80us and then up for about 80us
 */
uint8_t DHT11_Response(void)
{
	uint8_t response = 0;

	/* After initiasation of Data line, the line will stay  high for few us */
//	TIM14->CNT = 0x0000; // set timer count to 0 (zero)
//	HAL_TIM_Base_Start(&htim14);
//	while ((HAL_GPIO_ReadPin (DHT11_Out_GPIO_Port, DHT11_Out_Pin)));   // wait for the pin to go low
//	uint16_t timer_val_1 = __HAL_TIM_GET_COUNTER(&htim14);
//	HAL_TIM_Base_Stop(&htim14);	// stop the timer

	// Measure time when data line is Low
	// Check if the data line time is about 80us before it goes to High
	TIM14->CNT = 0x0000; // set timer count to 0 (zero)
	HAL_TIM_Base_Start(&htim14);
	while (!(HAL_GPIO_ReadPin (DHT11_Out_GPIO_Port, DHT11_Out_Pin)));   // wait for the pin to go High
	uint16_t time_interval_1 = __HAL_TIM_GET_COUNTER(&htim14);
	HAL_TIM_Base_Stop(&htim14);	// stop the timer

	// Measure time when data line is High
	// Check if the data line time is about 80us before it goes to Low again
	TIM14->CNT = 0x0000; // set timer count to 0 (zero)
	HAL_TIM_Base_Start(&htim14);
	while ((HAL_GPIO_ReadPin (DHT11_Out_GPIO_Port, DHT11_Out_Pin)));   // wait for the pin to go Low
	uint16_t time_interval_2 = __HAL_TIM_GET_COUNTER(&htim14);
	HAL_TIM_Base_Stop(&htim14);	// stop the timer

	// If both time is about 80us, response is OK from DHT11
	if (time_interval_1 > 60 && time_interval_2>60)
	{
		response = 1;
	}

	return response;
}

/*
 * Read data from DHT11
 */
uint8_t DHT11_GetData(void)
{
	uint8_t data;
	for (int j=0; j<8; j++)
	{
		// Wait till the pin goes high (50ms)
		while (!(HAL_GPIO_ReadPin (DHT11_Out_GPIO_Port, DHT11_Out_Pin)));
		// Once the Pin goes high -> start measuring the time
		TIM14->CNT = 0x0000; // set timer count to 0 (zero)
		HAL_TIM_Base_Start(&htim14);
		// Wait till the pin goes low (high voltage time tells whether the signal is 0 or 1)
		while ((HAL_GPIO_ReadPin (DHT11_Out_GPIO_Port, DHT11_Out_Pin)));
		uint16_t timer_interval = __HAL_TIM_GET_COUNTER(&htim14);
		HAL_TIM_Base_Stop(&htim14);	// stop the timer

		/* Get if the value read is 0 or 1 */
		if (timer_interval > 40){	// The value read is 1
			data |= (1<<(7-j));
		}
		else						// The value read is 0
		{
			data &= ~(1<<(7-j));
		}
	}

	return data;
}

// ================================= END OF FILE =================================
