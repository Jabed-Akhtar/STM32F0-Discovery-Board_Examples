#include "main.h"          // gives access to LED_Blue_Pin, etc.
#include "stm32f0xx_hal.h" // HAL GPIO functions

void app_entry(void)
{
    HAL_GPIO_TogglePin(GPIOC, LED_Blue_Pin);

    while (1)
    {
        HAL_Delay(1000);
        HAL_GPIO_TogglePin(GPIOC, LED_Blue_Pin | LED_Green_Pin);
    }
}
