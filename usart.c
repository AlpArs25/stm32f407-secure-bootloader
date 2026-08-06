#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "stm32f407xx.h"
#include "usart.h"
#include "gpio.h"
#include "rcc.h"

// We will have a default USART2 setup for simplicity
static HAL_Status usart2_pins_setup(void)
{
    GPIO_TypeDef *port;
    port = GPIOA;

    GPIO_Config tx_cfg = GPIO_CONFIG_DEFAULT;
    tx_cfg.af = USART2_AF;
    tx_cfg.mode = GPIO_MODE_AF;
    tx_cfg.pin = USART2_TX_PIN;

    GPIO_Config rx_cfg = GPIO_CONFIG_DEFAULT;
    rx_cfg.af = USART2_AF;
    rx_cfg.mode = GPIO_MODE_AF;
    rx_cfg.pin = USART2_RX_PIN;

    HAL_TRY(gpio_init(port, &tx_cfg));
    HAL_TRY(gpio_init(port, &rx_cfg));

    return HAL_OK;
}

HAL_Status usart_init(USART_TypeDef *usart, USART_Config *cfg)
{
    HAL_Status stat;
    if (usart == USART2)
    {
        RCC_USART2_CLK_ENABLE();
        stat = usart2_pins_setup();
    }
    else
    {
        return HAL_ERROR;
    }
    if (stat != HAL_OK)
    {
        return stat;
    }

    // BRR calculation
    uint32_t pclk = rcc_get_pclk1();
    uint32_t divisor = 16 * cfg->baud_rate;
    uint32_t mantissa = pclk / divisor;
    uint32_t remainder = pclk % divisor;
    uint32_t fraction = ((remainder * 16) + (divisor / 2)) / divisor;
    // Guard
    if (fraction > 15)
    {
        mantissa++;
        fraction = 0;
    }
    uint32_t brr = (mantissa << 4) | (fraction & 0xf);
    usart->BRR = brr;

    usart->CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;

    return HAL_OK;
}

HAL_Status usart_write(USART_TypeDef *usart, const uint8_t *buf, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        HAL_TRY(usart_write_byte(usart, buf[i]));
    }
    return HAL_OK;
}

HAL_Status usart_write_str(USART_TypeDef *usart, const char *text)
{
    return usart_write(usart, (const uint8_t *)text, strlen(text));
}

HAL_Status usart_write_byte(USART_TypeDef *usart, const char c)
{
    while (!(usart->SR & USART_SR_TXE))
    {
        // wait until empty
    }
    usart->DR = c;

    return HAL_OK;
}

HAL_Status usart_read(USART_TypeDef *usart, uint8_t *buf, size_t len)
{
    for (size_t i = 0; i < len - 1; i++)
    {
        HAL_TRY(usart_read_byte(usart, &buf[i]));
    }
    return HAL_OK;
}

HAL_Status usart_read_byte(USART_TypeDef *usart, uint8_t *c)
{
    while (!(usart->SR & USART_SR_RXNE))
    {
        // wait
    }
    if (usart->SR & USART_SR_ORE)
    {
        return HAL_ERROR;
    }
    *c = (char)(usart->DR & 0xFF);
    return HAL_OK;
}