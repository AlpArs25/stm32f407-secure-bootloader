#pragma once

#include "hal_common.h"
#include "stm32f407xx.h"

typedef struct
{
    uint32_t baud_rate;
} USART_Config;

HAL_Status usart_init(USART_TypeDef *usart, USART_Config *cfg);
HAL_Status usart_write(void);
HAL_Status usart_read(void);
