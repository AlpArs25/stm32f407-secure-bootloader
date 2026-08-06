#include <stdint.h>
#include "stm32f407xx.h"
#include "core_cm4.h"
#include "usart.h"
#include "gpio.h"
#include "hal_common.h"
#include <stdio.h>
#include "led.h"

int main(void)
{
    
    __enable_irq();
    systick_init(1000);
    HAL_CHECK(led_setup());

    while (1) 
    {
        led_toggle(LED_BLUE);
        delay_ms(1000);
    }
}