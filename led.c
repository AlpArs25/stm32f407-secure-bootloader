#include <stdint.h>
#include <stddef.h>
#include "stm32f407xx.h"
#include "core_cm4.h"
#include "hal_common.h"
#include "gpio.h"
#include "led.h"

HAL_Status led_setup(void)
{
    static const uint8_t led_pins[4] = {12, 13, 14, 15};

    for (size_t i = 0; i < 4; i++)
    {
        GPIO_Config cfg = GPIO_CONFIG_DEFAULT;
        cfg.mode = GPIO_MODE_OUTPUT;
        cfg.pin = led_pins[i];
        HAL_Status status = gpio_init(GPIOD, &cfg);
        if (status != HAL_OK)
        {
            return status;
        }
    }

    return HAL_OK;
}

HAL_Status led_on(LED_Color color)
{
    switch (color)
    {
    case LED_GREEN:
        GPIOD->BSRR = LED_GREEN_MASK;
        return HAL_OK;
        break;

    case LED_ORANGE:
        GPIOD->BSRR = LED_ORANGE_MASK;
        return HAL_OK;
        break;

    case LED_RED:
        GPIOD->BSRR = LED_RED_MASK;
        return HAL_OK;
        break;

    case LED_BLUE:
        GPIOD->BSRR = LED_BLUE_MASK;
        return HAL_OK;
        break;

    default:
        return HAL_ERROR;
    }
}

HAL_Status led_off(LED_Color color)
{
    switch (color)
    {
    case LED_GREEN:
        GPIOD->BSRR = (LED_GREEN_MASK << 16);
        return HAL_OK;
        break;

    case LED_ORANGE:
        GPIOD->BSRR = (LED_ORANGE_MASK << 16);
        return HAL_OK;
        break;

    case LED_RED:
        GPIOD->BSRR = (LED_RED_MASK << 16);
        return HAL_OK;
        break;

    case LED_BLUE:
        GPIOD->BSRR = (LED_BLUE_MASK << 16);
        return HAL_OK;
        break;

    default:
        return HAL_ERROR;
    }
}

HAL_Status led_toggle(LED_Color color)
{
    switch (color)
    {
    case LED_GREEN:
        GPIOD->ODR ^= LED_GREEN_MASK;
        return HAL_OK;
        break;

    case LED_ORANGE:
        GPIOD->ODR ^= LED_ORANGE_MASK;
        return HAL_OK;
        break;

    case LED_RED:
        GPIOD->ODR ^= LED_RED_MASK;
        return HAL_OK;
        break;

    case LED_BLUE:
        GPIOD->ODR ^= LED_BLUE_MASK;
        return HAL_OK;
        break;

    default:
        return HAL_ERROR;
    }
}