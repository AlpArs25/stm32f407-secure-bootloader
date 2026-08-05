#include <stdint.h>

extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t _estack;

typedef void (*isr_vector)(void);

void Reset_Handler(void);
void Default_Handler(void);

/* ---- ARM Cortex-M4 core exceptions (first 16) ---- */
void NMI_Handler(void) __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void) __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler(void) __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void) __attribute__((weak, alias("Default_Handler")));

/* ---- STM32F407 vendor interrupts (82 of them) ---- */
void WWDG_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void PVD_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TAMP_STAMP_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void RTC_WKUP_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void FLASH_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void RCC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI4_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream4_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream5_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream6_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void ADC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void CAN1_TX_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void CAN1_RX0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void CAN1_RX1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void CAN1_SCE_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI9_5_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM1_BRK_TIM9_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM1_UP_TIM10_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM1_TRG_COM_TIM11_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM1_CC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM4_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void I2C1_EV_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void I2C1_ER_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void I2C2_EV_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void I2C2_ER_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SPI1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SPI2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void USART1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void USART2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void USART3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI15_10_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void RTC_Alarm_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void OTG_FS_WKUP_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM8_BRK_TIM12_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM8_UP_TIM13_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM8_TRG_COM_TIM14_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM8_CC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream7_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void FSMC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SDIO_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM5_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SPI3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void UART4_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void UART5_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM6_DAC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM7_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream4_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void ETH_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void ETH_WKUP_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void CAN2_TX_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void CAN2_RX0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void CAN2_RX1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void CAN2_SCE_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void OTG_FS_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream5_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream6_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream7_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void USART6_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void I2C3_EV_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void I2C3_ER_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void OTG_HS_EP1_OUT_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void OTG_HS_EP1_IN_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void OTG_HS_WKUP_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void OTG_HS_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DCMI_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void CRYP_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void HASH_RNG_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void FPU_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));

__attribute__((section(".isr_vector")))
const isr_vector vector_table[] = {
    (isr_vector)&_estack, // 0:  initial stack pointer

    /* ---- ARM core exceptions (1-15) ---- */
    Reset_Handler,      // 1
    NMI_Handler,        // 2
    HardFault_Handler,  // 3
    MemManage_Handler,  // 4
    BusFault_Handler,   // 5
    UsageFault_Handler, // 6
    0,
    0,
    0,
    0,                // 7-10: reserved
    SVC_Handler,      // 11
    DebugMon_Handler, // 12
    0,                // 13: reserved
    PendSV_Handler,   // 14
    SysTick_Handler,  // 15

    /* ---- STM32F407 vendor interrupts (16-97) ---- */
    WWDG_IRQHandler,               // 16
    PVD_IRQHandler,                // 17
    TAMP_STAMP_IRQHandler,         // 18
    RTC_WKUP_IRQHandler,           // 19
    FLASH_IRQHandler,              // 20
    RCC_IRQHandler,                // 21
    EXTI0_IRQHandler,              // 22
    EXTI1_IRQHandler,              // 23
    EXTI2_IRQHandler,              // 24
    EXTI3_IRQHandler,              // 25
    EXTI4_IRQHandler,              // 26
    DMA1_Stream0_IRQHandler,       // 27
    DMA1_Stream1_IRQHandler,       // 28
    DMA1_Stream2_IRQHandler,       // 29
    DMA1_Stream3_IRQHandler,       // 30
    DMA1_Stream4_IRQHandler,       // 31
    DMA1_Stream5_IRQHandler,       // 32
    DMA1_Stream6_IRQHandler,       // 33
    ADC_IRQHandler,                // 34
    CAN1_TX_IRQHandler,            // 35
    CAN1_RX0_IRQHandler,           // 36
    CAN1_RX1_IRQHandler,           // 37
    CAN1_SCE_IRQHandler,           // 38
    EXTI9_5_IRQHandler,            // 39
    TIM1_BRK_TIM9_IRQHandler,      // 40
    TIM1_UP_TIM10_IRQHandler,      // 41
    TIM1_TRG_COM_TIM11_IRQHandler, // 42
    TIM1_CC_IRQHandler,            // 43
    TIM2_IRQHandler,               // 44
    TIM3_IRQHandler,               // 45
    TIM4_IRQHandler,               // 46
    I2C1_EV_IRQHandler,            // 47
    I2C1_ER_IRQHandler,            // 48
    I2C2_EV_IRQHandler,            // 49
    I2C2_ER_IRQHandler,            // 50
    SPI1_IRQHandler,               // 51
    SPI2_IRQHandler,               // 52
    USART1_IRQHandler,             // 53
    USART2_IRQHandler,             // 54
    USART3_IRQHandler,             // 55
    EXTI15_10_IRQHandler,          // 56
    RTC_Alarm_IRQHandler,          // 57
    OTG_FS_WKUP_IRQHandler,        // 58
    TIM8_BRK_TIM12_IRQHandler,     // 59
    TIM8_UP_TIM13_IRQHandler,      // 60
    TIM8_TRG_COM_TIM14_IRQHandler, // 61
    TIM8_CC_IRQHandler,            // 62
    DMA1_Stream7_IRQHandler,       // 63
    FSMC_IRQHandler,               // 64
    SDIO_IRQHandler,               // 65
    TIM5_IRQHandler,               // 66
    SPI3_IRQHandler,               // 67
    UART4_IRQHandler,              // 68
    UART5_IRQHandler,              // 69
    TIM6_DAC_IRQHandler,           // 70
    TIM7_IRQHandler,               // 71
    DMA2_Stream0_IRQHandler,       // 72
    DMA2_Stream1_IRQHandler,       // 73
    DMA2_Stream2_IRQHandler,       // 74
    DMA2_Stream3_IRQHandler,       // 75
    DMA2_Stream4_IRQHandler,       // 76
    ETH_IRQHandler,                // 77
    ETH_WKUP_IRQHandler,           // 78
    CAN2_TX_IRQHandler,            // 79
    CAN2_RX0_IRQHandler,           // 80
    CAN2_RX1_IRQHandler,           // 81
    CAN2_SCE_IRQHandler,           // 82
    OTG_FS_IRQHandler,             // 83
    DMA2_Stream5_IRQHandler,       // 84
    DMA2_Stream6_IRQHandler,       // 85
    DMA2_Stream7_IRQHandler,       // 86
    USART6_IRQHandler,             // 87
    I2C3_EV_IRQHandler,            // 88
    I2C3_ER_IRQHandler,            // 89
    OTG_HS_EP1_OUT_IRQHandler,     // 90
    OTG_HS_EP1_IN_IRQHandler,      // 91
    OTG_HS_WKUP_IRQHandler,        // 92
    OTG_HS_IRQHandler,             // 93
    DCMI_IRQHandler,               // 94
    CRYP_IRQHandler,               // 95
    HASH_RNG_IRQHandler,           // 96
    FPU_IRQHandler,                // 97
};

void Default_Handler(void)
{
    while (1)
    {
    }
}

extern int main(void);

void Reset_Handler(void)
{
    uint32_t *src;
    uint32_t *dst;
    src = (uint32_t *)&_sidata;
    dst = (uint32_t *)&_sdata;
    while (dst < (uint32_t *)&_edata)
    {
        *dst = *src;
        src++;
        dst++;
    }
    dst = (uint32_t *)&_sbss;
    while (dst < (uint32_t *)&_ebss)
    {
        *dst = 0U;
        dst++;
    }

    main();

    while (1)
    {
    }
}
