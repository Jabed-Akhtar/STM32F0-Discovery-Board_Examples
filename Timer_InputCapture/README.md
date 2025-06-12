# Timer_InputCapture

**Timers:**  
- TIM6: Used at app timer running with freq. 1 Hz. (Blinking Blue LED, Sending put message through UART)
- TIM2: Used for sending out PWM signal
- TIM3: Used as ***Input Capture***
## MCU and GPIOs
<img src="./evids/5_MCU_n_Pins.png" alt="-" width="400"/>

<img src="./evids/6_GPIOs.png" alt="-" width="500"/>

## Clock source
<img src="./evids/8_RCC.png" alt="-" width="600"/>

<img src="./evids/2_Timer_n_Clock.png" alt="-" width="800"/>

## NVICs
<img src="./evids/7_NVICs.png" alt="-" width="500"/>

## Timers
- Timers clock sources with frequencies can be seen in [Clock source](#clock-source)

<img src="./evids/1_Timer_n_Bus.png" alt="-" width="300"/>

### Timer for App
- This timer runs with freq. 1 Hz blinking LED and sending out messages via UART.

<img src="./evids/13_TIM6_params.png" alt="-" width="600"/>

<img src="./evids/14_TIM6_NVIC.png" alt="-" width="600"/>

### Timer for PWM
To test the ***Input Capture***, PWM signal is sent out using this Timer. PWM is configured with 10 kHz. PWM signal is sent out from Pin PA5. 10 kHz in time is: 100 us (microseconds). Basically, the timer goes HIGH -> LOW -> HIGH every 100 us (one full cycle).

<img src="./evids/3_PWM_Freq_calc.png" alt="-" width="300"/>

<img src="./evids/9_TIM2_params.png" alt="-" width="600"/>

<img src="./evids/10_TIM2_pins.png" alt="-" width="600"/>

<img src="./evids/11_TIM2_DMA.png" alt="-" width="600"/>

<img src="./evids/12_TIM2_NVIC.png" alt="-" width="600"/>

### Timer for Input Capture
10 kHz PWM signal, which is sent out from TIM2, is received here. This signal is received in Pin PA6. Here, the timer runs at freq. of 1 MHz. Time = 1 / 1 MHz = 1 us. This Input Capture Timer ticks every 1 us. To capture the incoming full PWM signal (rising edge to rising edge), this will tick 100 times.

<img src="./evids/15_TIM3_params.png" alt="-" width="600"/>

<img src="./evids/16_TIM3_GPIOs.png" alt="-" width="600"/>

<img src="./evids/17_TIM3_NVIC.png" alt="-" width="600"/>

## UART
UART is used for sending out messages, with for e.g. time difference between two rising edges.

<img src="./evids/18_UART_params.png" alt="-" width="600"/>

<img src="./evids/19_UART_GPIOs.png" alt="-" width="600"/>

## Schematics
<img src="./evids/Pins_Connections_Schematics.png" alt="-" width="500"/>

## Evidence
<img src="./evids/20_Evidence.png" alt="-" width="300"/>
