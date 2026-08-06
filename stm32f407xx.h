#pragma once

#include <stdint.h>

#define HSI_VALUE 16000000U

extern uint32_t SystemCoreClock;

typedef struct
{
    volatile uint32_t CR;
    volatile uint32_t PLLCFGR;
    volatile uint32_t CFGR;
    volatile uint32_t CIR;
    volatile uint32_t AHB1RSTR;
    volatile uint32_t AHB2RSTR;
    volatile uint32_t AHB3RSTR;
    uint32_t RESERVED0;
    volatile uint32_t APB1RSTR;
    volatile uint32_t APB2RSTR;
    uint32_t RESERVED1[2];
    volatile uint32_t AHB1ENR;
    volatile uint32_t AHB2ENR;
    volatile uint32_t AHB3ENR;
    uint32_t RESERVED2;
    volatile uint32_t APB1ENR;
    volatile uint32_t APB2ENR;
    // Rest
} RCC_TypeDef;

#define RCC_BASE 0x40023800U
#define RCC ((RCC_TypeDef *)(RCC_BASE))

#define RCC_CFGR_PPRE1_MASK (0x7 << 10)
#define RCC_CFGR_PPRE1_POS 10

typedef struct
{
    volatile uint32_t SR;
    volatile uint32_t DR;
    volatile uint32_t BRR;
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t GTPR;
} USART_TypeDef;

#define USART1_BASE 0x40011000U
#define USART1 ((USART_TypeDef *)(USART1_BASE))

#define USART2_BASE 0x40004400U
#define USART2 ((USART_TypeDef *)(USART2_BASE))

#define USART2_AF 7
#define USART2_RX_PIN 3
#define USART2_TX_PIN 2

#define USART_CR1_TE (1 << 3)
#define USART_CR1_RE (1 << 2)
#define USART_CR1_UE (1 << 13)
#define USART_SR_TXE (1 << 8)
#define USART_SR_RXNE (1 << 6)
#define USART_SR_ORE (1 << 3)


typedef struct
{
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t LCKR;
    volatile uint32_t AFR[2];
} GPIO_TypeDef;

#define GPIOA_BASE 0x40020000U
#define GPIOA ((GPIO_TypeDef *)(GPIOA_BASE))
#define GPIOB ((GPIO_TypeDef *)(GPIOA_BASE + 0x400))
#define GPIOC ((GPIO_TypeDef *)(GPIOA_BASE + 0x800))
#define GPIOD ((GPIO_TypeDef *)(GPIOA_BASE + 0xC00))

// Bit definitions
#define RCC_AHB1ENR_GPIOAEN (1 << 0)
#define RCC_AHB1ENR_GPIOBEN (1 << 1)
#define RCC_AHB1ENR_GPIOCEN (1 << 2)
#define RCC_AHB1ENR_GPIODEN (1 << 3)

#define RCC_APB1ENR_USART2EN (1 << 17)

#define RCC_GPIOA_CLK_ENABLE() (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN)
#define RCC_GPIOB_CLK_ENABLE() (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN)
#define RCC_GPIOC_CLK_ENABLE() (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN)
#define RCC_GPIOD_CLK_ENABLE() (RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN)

#define RCC_USART2_CLK_ENABLE() (RCC->APB1ENR |= RCC_APB1ENR_USART2EN)

typedef enum
{
    USART2_IRQn = 38,
    FPU_IRQn = 81,
    // Fill on demand
} IRQn_Type;
