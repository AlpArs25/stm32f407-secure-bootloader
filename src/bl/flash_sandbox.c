#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "stm32f407xx.h"
#include "core_cm4.h"
#include "flash.h"
#include "flash_sandbox.h"

#define PATTERN_ADDR (SANDBOX_BASE)
#define PATTERN_WORDS 8U

static const uint32_t pattern[PATTERN_WORDS] = {
    0xA5A5A5A5U,
    0x5A5A5A5AU,
    0xDEADBEEFU,
    0xCAFEBABEU,
    0x00112233U,
    0x44556677U,
    0x8899AABBU,
    0xCCDDEEFFU,
};

static void dump(const char *label, uint32_t addr, uint32_t words)
{
    const volatile uint32_t *p = (const volatile uint32_t *)(uintptr_t)addr;
    printf(" %s @ 0x%08lX:", label, (unsigned long)addr);
    for (uint32_t i = 0; i < words; i++)
    {
        printf(" %08lX", (unsigned long)p[i]);
    }
    printf("\r\n");
}

static void report(const char *what, HAL_Status st)
{
    if (st == HAL_OK)
    {
        printf(" %-34s OK\r\n", what);
    }
    else
    {
        printf(" %-34s FAIL (%s))", what, flash_error_str(flash_last_error()));
    }
}

HAL_Status flash_sandbox_run(void)
{
    printf("\r\n=== flash sandbox, sector %u @ 0x%08lX ===\r\n",
           (unsigned)SANDBOX_SECTOR, (unsigned long)SANDBOX_BASE);

    int already_written = (flash_verify(PATTERN_ADDR, pattern, sizeof(pattern)) == HAL_OK);

    if (!already_written)
    {
        printf("[1] pattern not present -> erase and write\r\n");

        uint32_t t0 = get_tick();
        HAL_Status st = flash_erase_sector(SANDBOX_SECTOR);
        uint32_t t1 = get_tick();
        report("erase sector", st);
        printf("      ticks observed across erase: %lu\r\n",
               (unsigned long)(t1 - t0));
        if (st != HAL_OK) { return st; }
        report("blank check (all 0xFF)",
               flash_is_erased(PATTERN_ADDR, sizeof(pattern)));
        dump("after erase", PATTERN_ADDR, PATTERN_WORDS);

        st = flash_program(PATTERN_ADDR, pattern, sizeof(pattern));
        report("program pattern", st);
        if (st != HAL_OK) { return st; }

        report("verify readback",
               flash_verify(PATTERN_ADDR, pattern, sizeof(pattern)));
        dump("after program", PATTERN_ADDR, PATTERN_WORDS);

        printf("\r\n  >>> now POWER CYCLE the board and run again <<<\r\n");
        return HAL_OK;
    }

    printf("[2] pattern survived power cycle\r\n");
    dump("persisted", PATTERN_ADDR, PATTERN_WORDS);

    printf("[3] program over non-erased flash\r\n");
    uint32_t overwrite = 0x0F0F0F0FU;
    printf("      current word0 = %08lX, writing %08lX\r\n",
           (unsigned long)*(volatile uint32_t *)PATTERN_ADDR,
           (unsigned long)overwrite);

    HAL_Status st = flash_program(PATTERN_ADDR, &overwrite, sizeof(overwrite));
    report("program without erase", st);
    printf("      result word0 = %08lX  (predict it before you look)\r\n",
           (unsigned long)*(volatile uint32_t *)PATTERN_ADDR);

    printf("[4] error paths\r\n");

    st = flash_program(BOOTLOADER_BASE, pattern, 4U);
    report("write to bootloader (guard)", st);

    st = flash_program(SANDBOX_BASE + 2U, pattern, 4U);
    report("misaligned address", st);

    st = flash_program(SANDBOX_BASE + 0x100U, pattern, 3U);
    report("length not multiple of 4", st);

    FLASH->SR = FLASH_SR_EOP | FLASH_SR_ERR_MASK;
    *(volatile uint32_t *)(SANDBOX_BASE + 0x200U) = 0x12345678U;
    printf("      SR after write with PG=0: 0x%08lX  (PGSERR is bit 7)\r\n",
           (unsigned long)FLASH->SR);
    FLASH->SR = FLASH_SR_EOP | FLASH_SR_ERR_MASK;

    printf("=== sandbox done ===\r\n\r\n");
    return HAL_OK;
}