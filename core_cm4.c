#include <stdint.h>
#include "core_cm4.h"
#include "stm32f407xx.h"
#include "hal_common.h"

static volatile uint32_t uwTick;

HAL_Status systick_init(uint32_t ticks_per_second)
{
    if (!ticks_per_second)
    {
        return HAL_ERROR;
    }
    uint32_t reload = ((SystemCoreClock / 8) / ticks_per_second) - 1;
    if (reload & 0XFF000000)
    {
        return HAL_ERROR;
    }
    SYSTICK->VAL = 0;
    SYSTICK->LOAD = (reload & 0X00FFFFFF);
    SYSTICK->CTRL |= SYSTICK_CTRL_TICKINT | SYSTICK_CTRL_EN;

    return HAL_OK;
}

void systick_dis(void)
{
    SYSTICK->CTRL &= ~(SYSTICK_CTRL_EN | SYSTICK_CTRL_TICKINT);
    SYSTICK->VAL = 0;
    SCB->ICSR = PENDSTCLR;
}

void SysTick_Handler(void)
{
    uwTick++;
}

uint32_t get_tick(void)
{
    return uwTick;
}

void delay_ms(uint32_t ms)
{
    uint32_t start = get_tick();
    while ((get_tick() - start) < ms)
    {
        // wait
    }
}