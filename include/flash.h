#pragma once

#include <stdint.h>
#include <stddef.h>
#include "hal_common.h"
#include "flash_map.h"

/* Local */
typedef enum
{
    FLASH_ERR_NONE = 0,
    FLASH_ERR_TIMEOUT,     /* BSY never cleared                      */
    FLASH_ERR_WRPERR,      /* write protected                        */
    FLASH_ERR_PGAERR,      /* alignment                              */
    FLASH_ERR_PGPERR,      /* PSIZE mismatch                         */
    FLASH_ERR_PGSERR,      /* CR not configured for this write       */
    FLASH_ERR_OPERR,       /* generic operation error                */
    FLASH_ERR_RANGE,       /* address guard rejected the target      */
    FLASH_ERR_ALIGN,       /* address or length not 4-byte aligned   */
    FLASH_ERR_LOCKED,      /* unlock sequence did not take           */
    FLASH_ERR_VERIFY,      /* readback did not match                 */
} Flash_Error;

HAL_Status flash_unlock(void);
HAL_Status flash_lock(void);

HAL_Status flash_erase_sector(uint32_t sector);
HAL_Status flash_program(uint32_t addr, const void *data, size_t len);

HAL_Status flash_verify(uint32_t addr, const void *expected, size_t len);
HAL_Status flash_is_erased(uint32_t addr, size_t len);

HAL_Status flash_addr_to_sector(uint32_t addr, uint32_t *sector_out);

Flash_Error flash_last_error(void);
const char *flash_error_str(Flash_Error err);