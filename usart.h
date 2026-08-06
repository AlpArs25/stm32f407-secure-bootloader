#pragma once

#include "hal_common.h"
#include "stm32f407xx.h"
#include <stddef.h>

typedef struct
{
    uint32_t baud_rate;
} USART_Config;

HAL_Status usart_init(USART_TypeDef *usart, USART_Config *cfg);
HAL_Status usart_write(USART_TypeDef *usart, const uint8_t *text, size_t len);
HAL_Status usart_write_str(USART_TypeDef *usart, const char *text);
HAL_Status usart_write_byte(USART_TypeDef *usart, const char c);
HAL_Status usart_read(USART_TypeDef *usart, uint8_t *buf, size_t len);
HAL_Status usart_read_byte(USART_TypeDef *usart, uint8_t *c);
