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
    HAL_CHECK(usart_init(USART2, &((USART_Config){.baud_rate = 115200})));
    setvbuf(stdout, NULL, _IONBF, 0); // debug
    while (1)
    {
        led_toggle(LED_BLUE);
        printf("test123");
        delay_ms(1000);
    }
}