#pragma once

#include <stdint.h>

#include "hal_common.h"
#include "stm32f407xx.h"

typedef struct {
    uint8_t pin;
    uint8_t mode;
    uint8_t pull;
    uint8_t speed;
    uint8_t af;
} GPIO_Config;

extern GPIO_Config const GPIO_Config_Default;

#define GPIO_MODE_INPUT 0
#define GPIO_MODE_OUTPUT 1
#define GPIO_MODE_AF 2
#define GPIO_MODE_ANALOG 3

#define GPIO_NOPULL     0
#define GPIO_PULLUP     1
#define GPIO_PULLDOWN   2

#define GPIO_SPEED_LOW     0
#define GPIO_SPEED_MEDIUM  1
#define GPIO_SPEED_HIGH    2
#define GPIO_SPEED_VHIGH   3

HAL_Status gpio_init(GPIO_TypeDef *port, GPIO_Config *cfg);