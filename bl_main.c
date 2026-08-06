#include "stm32f407xx.h"
#include "core_cm4.h"
#include <stdint.h>
#include "led.h"

#define APP_OFFSET 0x08020000

const uint32_t * const app_IV = (const uint32_t *)(APP_OFFSET);

volatile uint32_t g_bl_version __attribute__((used, section(".data.keep"))) = 0x00010000; // Hacky objcopy fix

int main(void)
{
    systick_init(1000);
    led_setup();
    for(int i = 0; i < 3; i++)
    {
        led_toggle(ORANGE);
        delay_ms(500);
    }
    led_off(ORANGE);
    systick_dis();

    void *app_entry;
    uint32_t app_end_stack;

    __disable_irq();

    SCB->VTOR = (uint32_t)app_IV;
    __DSB();
    __ISB();

    app_end_stack = (*(volatile uint32_t *)(APP_OFFSET));
    app_entry = (void *)(*(volatile uint32_t *)(APP_OFFSET + 0x4));


    // need one line
    asm volatile("msr msp, %0 \n bx %1" :: "r"(app_end_stack), "r"(app_entry));

    // asm volatile("msr msp, %0" :: "r"(app_end_stack));
    // asm volatile("mov pc, %0" :: "r"(app_entry));

    while (1) {}
}