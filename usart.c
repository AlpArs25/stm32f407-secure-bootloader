#include <stdint.h>
#include <stddef.h>
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

    gpio_init(port, &tx_cfg);
    gpio_init(port, &rx_cfg);

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
    uint32_t pclk = hal_rcc_get_pclk1();
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

HAL_Status usart_write(USART_TypeDef *usart, const char *text)
{
    const char *c = text;
    while (*c != '\0')
    {
        HAL_Status status = usart_write_byte(usart, c);
        if (status != HAL_OK)
        {
            return status;
        }
        c++;
    }
    return HAL_OK;
}

HAL_Status usart_write_byte(USART_TypeDef *usart, const char *c)
{
    if (usart == USART2)
    {
        while (!(USART2->SR & USART_SR_TXE))
        {
            // wait until empty
        }
        USART2->DR = *c;

        return HAL_OK;
    } // rest
    return HAL_ERROR;
}

HAL_Status usart_read(USART_TypeDef *usart, char *buf, size_t len)
{
    if (len == 0)
    {
        return HAL_ERROR;
    }
    for (size_t i = 0; i < len - 1; i++)
    {
        HAL_Status status = usart_read_byte(usart, &buf[i]);
        if (status != HAL_OK)
        {
            return status;
        }
    }
    buf[len - 1] = '\0';
    return HAL_OK;
}

HAL_Status usart_read_byte(USART_TypeDef *usart, char *c)
{
    if (usart == USART2)
    {
        while (!(USART2->SR & USART_SR_RXNE))
        {
            // wait
        }
        *c = (char)(USART2->DR & 0xFF);
        return HAL_OK;
    }
    return HAL_ERROR;
}