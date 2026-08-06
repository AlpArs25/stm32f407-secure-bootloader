#pragma once

#include <stdint.h>
#include "stm32f407xx.h"
#include "core_cm4.h"
#include "hal_common.h"
#include "gpio.h"

typedef enum
{
    GREEN = 0,
    ORANGE = 1,
    RED = 2,
    BLUE = 3,
} LED_COLOR;

#define LED_GREEN_EN (1 << 12)
#define LED_ORANGE_EN (1 << 13)
#define LED_RED_EN (1 << 14)
#define LED_BLUE_EN (1 << 15)


HAL_Status led_setup(void);
HAL_Status led_on(LED_COLOR color);
HAL_Status led_off(LED_COLOR color);
HAL_Status led_toggle(LED_COLOR color);
