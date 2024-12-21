/* USER CODE BEGIN Header */
/*
 * LCD Display 1602A
 * Some code taken from: https://controllerstech.com/i2c-lcd-in-stm32/
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LCD_IIC_ADDR 0x27 << 1 	// ADDR 0x27<<1 Checked!
#define LCD_BACKLIGHT 0x08   	// Backlight ON/OFF
#define LCD_ENABLE 0x04      	// Enable bit
#define LCD_RS 0x01          	// Selects registers (Instruction or Data register)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
DMA_HandleTypeDef hdma_i2c1_tx;

TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim14;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_tx;

/* USER CODE BEGIN PV */
uint8_t counter_periodic = 0;
uint8_t counter_button_pressed = 0;
char uart_tx_buffer_periodic[50];
char uart_tx_buffer_counter_button_pressed[50];
char iic_tx_buffer_periodic[16];
char iic_tx_buffer_counter_button_pressed[16];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM6_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM14_Init(void);
/* USER CODE BEGIN PFP */
void lcd_iic_send_cmd (char cmd)
{
	char data_u, data_l;
	data_u = (cmd&0xf0);
	data_l = ((cmd<<4)&0xf0);
	uint8_t data_t[4];
	data_t[0] = data_u|0x0C;  //en=1, rs=0 -> bxxxx1100
	data_t[1] = data_u|0x08;  //en=0, rs=0 -> bxxxx1000
	data_t[2] = data_l|0x0C;  //en=1, rs=0 -> bxxxx1100
	data_t[3] = data_l|0x08;  //en=0, rs=0 -> bxxxx1000
	HAL_I2C_Master_Transmit (&hi2c1, LCD_IIC_ADDR,(uint8_t *) data_t, 4, 100);
}

void lcd_iic_send_data (char data)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data&0xf0);
	data_l = ((data<<4)&0xf0);
	data_t[0] = data_u|0x0D;  //en=1, rs=1 -> bxxxx1101
	data_t[1] = data_u|0x09;  //en=0, rs=1 -> bxxxx1001
	data_t[2] = data_l|0x0D;  //en=1, rs=1 -> bxxxx1101
	data_t[3] = data_l|0x09;  //en=0, rs=1 -> bxxxx1001
	HAL_I2C_Master_Transmit (&hi2c1, LCD_IIC_ADDR,(uint8_t *) data_t, 4, 100);
}

void lcd_iic_init (void)
{
  Delay_Ms(150);  // wait some extra ms
  // 4 bit initialisation
  Delay_Ms(50);  // wait for >40ms
  lcd_iic_send_cmd (0x30);
  Delay_Ms(5);  // wait for >4.1ms
  lcd_iic_send_cmd (0x30);
  Delay_Ms(1);  // wait for >100us
  lcd_iic_send_cmd (0x30);
  Delay_Ms(10);
  lcd_iic_send_cmd (0x20);  // 4bit mode
  Delay_Ms(10);

  // display initialisation
  lcd_iic_send_cmd (0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
  Delay_Ms(1);
  lcd_iic_send_cmd (0x08); //Display on/off control --> D=0,C=0, B=0  ---> display off
  Delay_Ms(1);
  lcd_iic_send_cmd (0x01);  // clear display
  Delay_Ms(2);
  lcd_iic_send_cmd (0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
  Delay_Ms(1);
  lcd_iic_send_cmd (0x0C); //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
}

void lcd_iic_send_string (char *str)
{
  while (*str) lcd_iic_send_data (*str++);
}

void lcd_iic_put_cursor(int row, int col)
{
    switch (row)
    {
        case 0:
            col |= 0x80;
            break;
        case 1:
            col |= 0xC0;
            break;
    }
    lcd_iic_send_cmd (col);
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_TIM6_Init();
  MX_USART1_UART_Init();
  MX_TIM14_Init();
  /* USER CODE BEGIN 2 */
  // Send init msg through UART
  uint8_t msg_init[] = "Program started.\r\n";
  HAL_UART_Transmit_DMA(&huart1, msg_init, sizeof(msg_init));

  // initilize lcd display and send string to Display
  lcd_iic_init ();
  lcd_iic_put_cursor(0, 0);
  lcd_iic_send_string ("Program started.");

  // Start timer interrupt (1 sec)
  HAL_TIM_Base_Start_IT(&htim14);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  __WFI();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00201D2B;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 48-1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 65535-1;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief TIM14 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM14_Init(void)
{

  /* USER CODE BEGIN TIM14_Init 0 */

  /* USER CODE END TIM14_Init 0 */

  /* USER CODE BEGIN TIM14_Init 1 */

  /* USER CODE END TIM14_Init 1 */
  htim14.Instance = TIM14;
  htim14.Init.Prescaler = 48000-1;
  htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim14.Init.Period = 1000-1;
  htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM14_Init 2 */

  /* USER CODE END TIM14_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel2_3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
  /* DMA1_Channel4_5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LED_Blue_Pin|LED_Green_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_Blue_Pin LED_Green_Pin */
  GPIO_InitStruct.Pin = LED_Blue_Pin|LED_Green_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/*
 * Callback function on Push-Button clicked.
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	HAL_GPIO_TogglePin(GPIOC, LED_Green_Pin); // Toggle Green LED

	// Send out message via UART
	sprintf(uart_tx_buffer_counter_button_pressed,
			"Button pressed counter: %d\r\n",
			counter_button_pressed
			);
	HAL_UART_Transmit_DMA(&huart1,
			uart_tx_buffer_counter_button_pressed,
			sizeof(uart_tx_buffer_counter_button_pressed)
			);

	// Send message to LCD display
	sprintf(iic_tx_buffer_counter_button_pressed, "Btn-Pressed: %d", counter_periodic);
	lcd_iic_put_cursor(1, 0);
	lcd_iic_send_string (iic_tx_buffer_counter_button_pressed);

	counter_button_pressed++;
	if (counter_button_pressed>=255) // Reset counter when reaching value 255
	{
		counter_button_pressed = 0;
		lcd_iic_init();
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/* Prevent unused argument(s) compilation warning */
	  UNUSED(htim);

	  // Toggle the LEDs
	  if (htim->Instance == TIM14){
		  	HAL_StatusTypeDef status_uart;

		  	HAL_GPIO_TogglePin(GPIOC, LED_Blue_Pin);

		  	sprintf(uart_tx_buffer_periodic, "Periodic counter: %d\r\n", counter_periodic);
		  	HAL_UART_Transmit_DMA(&huart1, uart_tx_buffer_periodic, sizeof(uart_tx_buffer_periodic));

		  	sprintf(iic_tx_buffer_periodic, "Tim-Counter: %d", counter_periodic);
//		  	lcd_iic_init();
		  	lcd_iic_put_cursor(0, 0);
			lcd_iic_send_string (iic_tx_buffer_periodic);

		  	counter_periodic++;
		  	if (counter_periodic>=255) // Reset counter when reaching value 255
		  	{
		  		counter_periodic = 0;
		  		lcd_iic_init();
		  	}
	  }
}

/*
 * Fun: Delay in us (micro-second)
 * Reset the Counter
 * Wait for the Counter to reach the input value (us)
 * Each Count take 1 us.
 */
void Delay_Us(uint16_t us)
{
	TIM6->CNT = 0x0000; // set timer count to 0 (zero)
	//__HAL_TIM_SET_COUNTER(&htim6, 0); // Reset the counter
	HAL_TIM_Base_Start(&htim6);	// start the timer
//	while (TIM6->CNT < us);
	while (__HAL_TIM_GET_COUNTER(&htim6) < us); // Wait until the counter reaches the delay value
	HAL_TIM_Base_Stop(&htim6);	// stop the timer
}

void Delay_Ms(uint16_t sec)
{
	for (uint16_t i=0; i<sec; i++)
	{
		Delay_Us(1000);	// delay of 1 ms
	}
}

void Delay_S(uint16_t sec)
{
	for (uint16_t i=0; i<sec; i++)
	{
		Delay_Ms(1000);	// delay of 1 sec
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
