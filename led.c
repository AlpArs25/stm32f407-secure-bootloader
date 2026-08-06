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

static const uint16_t led_mask[] = {LED_GREEN_MASK, LED_ORANGE_MASK, LED_RED_MASK, LED_BLUE_MASK};

HAL_Status led_on(LED_Color color)
{
    if (color >= LED_COUNT)
    {
        return HAL_ERROR;
    }
    GPIOD->BSRR = led_mask[color];
    return HAL_OK;
}

HAL_Status led_off(LED_Color color)
{
    if (color >= LED_COUNT)
    {
        return HAL_ERROR;
    }
    GPIOD->BSRR = (led_mask[color] << 16);
    return HAL_OK;
}

HAL_Status led_toggle(LED_Color color)
{
    if (color >= LED_COUNT)
    {
        return HAL_ERROR;
    }
    GPIOD->ODR ^= led_mask[color];
    return HAL_OK;
}
