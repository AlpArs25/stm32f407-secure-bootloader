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

HAL_Status led_on(LED_COLOR color)
{
    switch (color)
    {
    case GREEN:
        GPIOD->BSRR = LED_GREEN_EN;
        return HAL_OK;
        break;

    case ORANGE:
        GPIOD->BSRR = LED_ORANGE_EN;
        return HAL_OK;
        break;

    case RED:
        GPIOD->BSRR = LED_RED_EN;
        return HAL_OK;
        break;

    case BLUE:
        GPIOD->BSRR = LED_BLUE_EN;
        return HAL_OK;
        break;

    default:
        return HAL_ERROR;
    }
}

HAL_Status led_off(LED_COLOR color)
{
    switch (color)
    {
    case GREEN:
        GPIOD->BSRR = (LED_GREEN_EN << 16);
        return HAL_OK;
        break;

    case ORANGE:
        GPIOD->BSRR = (LED_ORANGE_EN << 16);
        return HAL_OK;
        break;

    case RED:
        GPIOD->BSRR = (LED_RED_EN << 16);
        return HAL_OK;
        break;

    case BLUE:
        GPIOD->BSRR = (LED_BLUE_EN << 16);
        return HAL_OK;
        break;

    default:
        return HAL_ERROR;
    }
}

HAL_Status led_toggle(LED_COLOR color)
{
    switch (color)
    {
    case GREEN:
        GPIOD->ODR ^= LED_GREEN_EN;
        return HAL_OK;
        break;

    case ORANGE:
        GPIOD->ODR ^= LED_ORANGE_EN;
        return HAL_OK;
        break;

    case RED:
        GPIOD->ODR ^= LED_RED_EN;
        return HAL_OK;
        break;

    case BLUE:
        GPIOD->ODR ^= LED_BLUE_EN;
        return HAL_OK;
        break;

    default:
        return HAL_ERROR;
    }
}