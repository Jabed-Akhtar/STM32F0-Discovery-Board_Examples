
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "iic_slave.h"

/* Defines -------------------------------------------------------------------*/
#define RxSIZE 6

/* Variables -----------------------------------------------------------------*/
extern I2C_HandleTypeDef hi2c1;

int counter = 0;
uint8_t RxData[RxSIZE];

/* Functions -----------------------------------------------------------------*/
/*
 * Callback function for HAL_I2C_EnableListen_IT
 */
void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c)
{
	HAL_I2C_EnableListen_IT(&hi2c1);	// Put IIC back to listen mode
}

/*
 * Callback function for IIC-Slave-Address-matched
 */
void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode)
{
	if (TransferDirection == I2C_DIRECTION_TRANSMIT)
	{
		HAL_I2C_Slave_Seq_Receive_IT(&hi2c1, RxData, RxSIZE, I2C_FIRST_AND_LAST_FRAME);

		if (RxData[1]==0x0 && RxData[2]==0x0 && RxData[3]==0x0  && RxData[4]==0x0 && RxData[5]==0x0)
		{
			HAL_GPIO_TogglePin(GPIOC, LD3_Pin);	// Toggle Green LED
		}
		else
		{
			HAL_GPIO_TogglePin(GPIOC, LD4_Pin);	// Toggle Blue LED
		}
	}
	else
	{
		Error_Handler();
	}
}

/*
 * Callback function IIC-Slave-Rx-Transfer completion.
 */
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	counter++;
}

/*
 * Callback function for IIC Error
 */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
	HAL_I2C_EnableListen_IT(&hi2c1);
}

// ================================= END OF FILE =================================
