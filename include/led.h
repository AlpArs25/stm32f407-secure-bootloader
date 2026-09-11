#pragma once

#include <stdint.h>
#include "stm32f407xx.h"
#include "hal_common.h"
#include "gpio.h"

typedef enum
{
    LED_GREEN = 0,
    LED_ORANGE = 1,
    LED_RED = 2,
    LED_BLUE = 3,
    LED_COUNT,
} LED_Color;

#define LED_GREEN_PIN 12
#define LED_ORANGE_PIN 13
#define LED_RED_PIN 14
#define LED_BLUE_PIN 15

#define LED_GREEN_MASK (1U << LED_GREEN_PIN)
#define LED_ORANGE_MASK (1U << LED_ORANGE_PIN)
#define LED_RED_MASK (1U << LED_RED_PIN)
#define LED_BLUE_MASK (1U << LED_BLUE_PIN)

HAL_Status led_setup(void);
HAL_Status led_on(LED_Color color);
HAL_Status led_off(LED_Color color);
HAL_Status led_toggle(LED_Color color);
