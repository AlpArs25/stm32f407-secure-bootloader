#pragma once

#include <stdint.h>
#include "stm32f407xx.h"
#include "hal_common.h"

#define VTOR ((volatile uint32_t *)(0xE000ED08U))
#define NVIC ((NVIC_TypeDef *)(0xE000E100U))
#define CPACR ((volatile uint32_t *)(0xE000ED88U))

#define FPU_ENABLE (0xF << 20)

typedef struct
{
    volatile uint32_t ISER[8];
    uint32_t RESERVED0[24];
    volatile uint32_t ICER[8];
    uint32_t RESERVED1[24];
    volatile uint32_t ISPR[8];
    uint32_t RESERVED2[24];
    volatile uint32_t ICPR[8];
} NVIC_TypeDef;

typedef struct
{
    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t VAL;
    volatile uint32_t CALIB;

} SysTick_TypeDef;

#define SYSTICK ((SysTick_TypeDef *)(0xE000E010U))

#define SYSTICK_CTRL_EN (1U << 0)
#define SYSTICK_CTRL_TICKINT (1U << 1)

HAL_Status systick_init(uint32_t ticks_per_second);
void SysTick_Handler(void);
uint32_t get_tick(void);
void delay_ms(uint32_t ms);

static inline void nvic_irq_enable(IRQn_Type n)
{
    uint8_t reg_offset = n / 32;
    NVIC->ISER[reg_offset] = 1 << (n % 32);
}

static inline void nvic_irq_disable(IRQn_Type n)
{
    uint8_t reg_offset = n / 32;
    NVIC->ICER[reg_offset] = 1 << (n % 32);
}

static inline void __disable_irq(void)
{
    asm volatile("cpsid i" ::: "memory"); // "memory" clobber to prevent reordering
}

static inline void __enable_irq(void)
{
    asm volatile("cpsie i" ::: "memory");
}

static inline void __ISB(void)
{
    asm volatile("isb 0xf" ::: "memory");
}

static inline void __DMB(void)
{
    asm volatile("dmb 0xf" ::: "memory");
}

static inline void __DSB(void)
{
    asm volatile("dsb 0xf" ::: "memory");
}
