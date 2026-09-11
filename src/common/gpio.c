#include "gpio.h"
#include "stm32f407xx.h"

const GPIO_Config GPIO_CONFIG_DEFAULT = {
    .pin = 0,
    .mode = GPIO_MODE_INPUT,
    .pull = GPIO_NOPULL,
    .speed = GPIO_SPEED_LOW,
    .af = 0,
    .otype = 0};

HAL_Status gpio_init(GPIO_TypeDef *port, const GPIO_Config *cfg)
{
    if (port == GPIOA)
    {
        RCC_GPIOA_CLK_ENABLE();
    }
    else if (port == GPIOD)
    {
        RCC_GPIOD_CLK_ENABLE();
    } // Rest
    uint8_t pin = cfg->pin;
    port->MODER &= ~(3 << (pin * 2));
    port->MODER |= cfg->mode << (pin * 2);
    port->OSPEEDR &= ~(3 << (pin * 2));
    port->OSPEEDR |= cfg->speed << (pin * 2);
    port->PUPDR &= ~(3 << (pin * 2));
    port->PUPDR |= cfg->pull << (pin * 2);
    port->OTYPER &= ~(1 << pin);
    port->OTYPER |= cfg->otype << pin;

    uint8_t reg_select = pin / 8;
    pin = pin % 8;
    port->AFR[reg_select] &= ~(0xf << (pin * 4));
    port->AFR[reg_select] |= cfg->af << (pin * 4);

    return HAL_OK;
}
