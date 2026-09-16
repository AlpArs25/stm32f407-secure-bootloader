#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "stm32f407xx.h"
#include "core_cm4.h"
#include "flash.h"

const Flash_Sector flash_sectors[FLASH_SECTOR_COUNT] = {
    {0x08000000U, 16U * 1024U},  /*  0 */
    {0x08004000U, 16U * 1024U},  /*  1 */
    {0x08008000U, 16U * 1024U},  /*  2 */
    {0x0800C000U, 16U * 1024U},  /*  3 */
    {0x08010000U, 64U * 1024U},  /*  4  metadata  */
    {0x08020000U, 128U * 1024U}, /*  5  slot A    */
    {0x08040000U, 128U * 1024U}, /*  6  slot A    */
    {0x08060000U, 128U * 1024U}, /*  7  slot B    */
    {0x08080000U, 128U * 1024U}, /*  8  slot B    */
    {0x080A0000U, 128U * 1024U}, /*  9  sandbox   */
    {0x080C0000U, 128U * 1024U}, /* 10            */
    {0x080E0000U, 128U * 1024U}, /* 11            */
};

#define ERASE_TIMEOUT_MS 4000U
#define PROGRAM_TIMEOUT_MS 50U

static Flash_Error last_error = FLASH_ERR_NONE;

static void clear_flags(void)
{
    FLASH->SR = FLASH_SR_EOP | FLASH_SR_ERR_MASK;
}

static HAL_Status wait_ready(uint32_t timeout_ms)
{
    uint32_t start = get_tick();

    while (FLASH->SR & FLASH_SR_BSY)
    {
        if ((get_tick() - start) > timeout_ms)
        {
            last_error = FLASH_ERR_TIMEOUT;
            return HAL_TIMEOUT;
        }
    }

    uint32_t sr = FLASH->SR;
    HAL_Status status = HAL_OK;

    if (sr & FLASH_SR_WRPERR)
    {
        last_error = FLASH_ERR_WRPERR;
        status = HAL_ERROR;
    }
    else if (sr & FLASH_SR_PGAERR)
    {
        last_error = FLASH_ERR_PGAERR;
        status = HAL_ERROR;
    }
    else if (sr & FLASH_SR_PGPERR)
    {
        last_error = FLASH_ERR_PGPERR;
        status = HAL_ERROR;
    }
    else if (sr & FLASH_SR_PGSERR)
    {
        last_error = FLASH_ERR_PGSERR;
        status = HAL_ERROR;
    }
    else if (sr & FLASH_SR_OPERR)
    {
        last_error = FLASH_ERR_OPERR;
        status = HAL_ERROR;
    }

    clear_flags();
    return status;
}

/* ---- address guard -------------------------------------------------
 * The only regions this image is permitted to modify. The bootloader
 * stages images into slot B and uses scratch for bring-up; the app
 * (stage 4) will only ever touch the metadata sector. Anything else —
 * a corrupt address in a received frame, a runaway loop — is refused
 * before a single hardware register is touched.
 * ------------------------------------------------------------------ */

static int region_contains(uint32_t addr, size_t len, uint32_t base, uint32_t size)
{
    if (addr < base)
    {
        return 0;
    }
    if ((uint64_t)addr + (uint64_t)len > (uint64_t)base + (uint64_t)size)
    {
        return 0;
    }
    return 1;
}

static HAL_Status guard(uint32_t addr, size_t len)
{
    #if defined(IMAGE_BOOTLOADER)
        if (region_contains(addr, len, SLOT_B_BASE, SLOT_B_SIZE) ||
            region_contains(addr, len, SCRATCH_BASE, SCRATCH_SIZE))
        {
            return HAL_OK;
        }
    #elif defined(IMAGE_APP)
        if (region_contains(addr, len, METADATA_BASE, METADATA_SIZE))
        {
            return HAL_OK;
        }
    #else
    #error "Build must define IMAGE_BOOTLOADER or IMAGE_APP"
    #endif

    last_error = FLASH_ERR_RANGE;
    return HAL_ERROR;
}

/* ---- unlock / lock ------------------------------------------------- */

HAL_Status flash_unlock(void)
{
    if (FLASH->CR & FLASH_CR_LOCK)
    {
        FLASH->KEYR = FLASH_KEY1;
        FLASH->KEYR = FLASH_KEY2;
    }
    if (FLASH->CR & FLASH_CR_LOCK)
    {
        last_error = FLASH_ERR_LOCKED;
        return HAL_ERROR;
    }
    return HAL_OK;
}

HAL_Status flash_lock(void)
{
    FLASH->CR |= FLASH_CR_LOCK;
    return HAL_OK;
}

/* ---- sector helpers ------------------------------------------------ */

HAL_Status flash_addr_to_sector(uint32_t addr, uint32_t *sector_out)
{
    if (sector_out == NULL)
    {
        return HAL_ERROR;
    }
    for (uint32_t i = 0; i < FLASH_SECTOR_COUNT; i++)
    {
        if (addr >= flash_sectors[i].base &&
            addr < flash_sectors[i].base + flash_sectors[i].size)
        {
            *sector_out = i;
            return HAL_OK;
        }
    }
    last_error = FLASH_ERR_RANGE;
    return HAL_ERROR;
}

/* ---- erase --------------------------------------------------------- */

HAL_Status flash_erase_sector(uint32_t sector)
{
    if (sector >= FLASH_SECTOR_COUNT)
    {
        last_error = FLASH_ERR_RANGE;
        return HAL_ERROR;
    }

    HAL_TRY(guard(flash_sectors[sector].base, flash_sectors[sector].size));

    /* Stale flags from a previous op would be misread as ours. */
    clear_flags();
    HAL_TRY(flash_unlock());

    FLASH->CR &= ~FLASH_CR_PSIZE_MASK;
    FLASH->CR |= FLASH_CR_PSIZE_X32; /* sets erase parallelism too */
    FLASH->CR &= ~FLASH_CR_SNB_MASK;
    FLASH->CR |= (sector << FLASH_CR_SNB_POS) & FLASH_CR_SNB_MASK;
    FLASH->CR |= FLASH_CR_SER;
    FLASH->CR |= FLASH_CR_STRT;

    /* The core freezes here until the array is free. */
    HAL_Status status = wait_ready(ERASE_TIMEOUT_MS);

    FLASH->CR &= ~FLASH_CR_SER;
    (void)flash_lock(); /* lock even on the error path */

    return status;
}

/* ---- program ------------------------------------------------------- */

HAL_Status flash_program(uint32_t addr, const void *data, size_t len)
{
    if (data == NULL || len == 0U)
    {
        return HAL_ERROR;
    }
    if ((addr % 4U != 0U) || (len % 4U != 0U))
    {
        last_error = FLASH_ERR_ALIGN;
        return HAL_ERROR;
    }

    HAL_TRY(guard(addr, len));

    clear_flags();
    HAL_TRY(flash_unlock());

    /* PSIZE must match how we store below, every time. */
    FLASH->CR &= ~FLASH_CR_PSIZE_MASK;
    FLASH->CR |= FLASH_CR_PSIZE_X32;
    FLASH->CR |= FLASH_CR_PG;

    const uint8_t *src = (const uint8_t *)data;
    volatile uint32_t *dst = (volatile uint32_t *)addr;
    size_t words = len / 4U;
    HAL_Status status = HAL_OK;

    for (size_t i = 0; i < words; i++)
    {
        uint32_t word;
        /* memcpy, not a uint32_t* cast: the caller's buffer may not be
         * 4-byte aligned (a protocol payload inside a frame, say), and
         * dereferencing a misaligned uint32_t* is undefined behaviour.
         * GCC compiles this to a single load when alignment permits. */
        memcpy(&word, src + (i * 4U), sizeof(word));

        *dst = word;
        dst++;

        status = wait_ready(PROGRAM_TIMEOUT_MS);
        if (status != HAL_OK)
        {
            break; /* stop at the first bad word; caller learns where */
        }
    }

    FLASH->CR &= ~FLASH_CR_PG;
    (void)flash_lock();

    return status;
}

/* ---- read-side helpers --------------------------------------------- */

HAL_Status flash_verify(uint32_t addr, const void *expected, size_t len)
{
    if (memcmp((const void *)(uintptr_t)addr, expected, len) != 0)
    {
        last_error = FLASH_ERR_VERIFY;
        return HAL_ERROR;
    }
    return HAL_OK;
}

HAL_Status flash_is_erased(uint32_t addr, size_t len)
{
    const uint8_t *p = (const uint8_t *)(uintptr_t)addr;
    for (size_t i = 0; i < len; i++)
    {
        if (p[i] != FLASH_ERASED_BYTE)
        {
            last_error = FLASH_ERR_VERIFY;
            return HAL_ERROR;
        }
    }
    return HAL_OK;
}

/* ---- diagnostics --------------------------------------------------- */

Flash_Error flash_last_error(void)
{
    return last_error;
}

const char *flash_error_str(Flash_Error err)
{
    switch (err)
    {
    case FLASH_ERR_NONE:
        return "none";
    case FLASH_ERR_TIMEOUT:
        return "BSY timeout";
    case FLASH_ERR_WRPERR:
        return "write protected";
    case FLASH_ERR_PGAERR:
        return "alignment error";
    case FLASH_ERR_PGPERR:
        return "parallelism (PSIZE) error";
    case FLASH_ERR_PGSERR:
        return "sequence error (CR not configured)";
    case FLASH_ERR_OPERR:
        return "operation error";
    case FLASH_ERR_RANGE:
        return "address outside permitted region";
    case FLASH_ERR_ALIGN:
        return "address/length not word aligned";
    case FLASH_ERR_LOCKED:
        return "unlock failed";
    case FLASH_ERR_VERIFY:
        return "readback mismatch";
    default:
        return "unknown";
    }
}