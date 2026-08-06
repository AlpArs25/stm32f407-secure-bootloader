#include <stdint.h>
#include "stm32f407xx.h"
#include "rcc.h"

uint32_t hal_rcc_get_pclk1(void)
{
    uint32_t ppre1 = (RCC->CFGR & RCC_CFGR_PPRE1_MASK) >> RCC_CFGR_PPRE1_POS;

    // if top bit clear
    uint32_t divider = 1;

    if (ppre1 & 0x4)
    {
        static const uint32_t ppre1_dividers[4] = {2, 4, 8, 16};
        divider = ppre1_dividers[ppre1 & 0x3];
    }

    return SystemCoreClock / divider;
}