#pragma once

#include <stdint.h>

#define HSI_VALUE   16000000U

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

// Bit definitions
#define RCC_AHB1ENR_GPIOAEN (1 << 0)
#define RCC_APB1ENR_USART2EN (1 << 17)

#define RCC_GPIOA_CLK_ENABLE() (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN)
#define RCC_USART2_CLK_ENABLE() (RCC->APB1ENR |= RCC_APB1ENR_USART2EN)


#define USART2_AF 7
#define USART2_RX_PIN 3
#define USART2_TX_PIN 2

#define USART_CR1_TE (1 << 3)
#define USART_CR1_RE (1 << 2)
#define USART_CR1_UA (1 << 13)


