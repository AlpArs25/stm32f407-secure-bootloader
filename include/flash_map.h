#pragma once

#include <stdint.h>

/* ---------------------------------------------------------------------
 * STM32F407VG flash layout. 1 MB, 12 sectors, non-uniform (design rule 6).
 *
 *   Sector | Base        | Size  | Region
 *     0-3  | 0x08000000  | 16K   | Bootloader (64K)
 *     4    | 0x08010000  | 64K   | Metadata
 *     5-6  | 0x08020000  | 128K  | Slot A  (execution slot, 256K)
 *     7-8  | 0x08060000  | 128K  | Slot B  (staging slot,   256K)
 *     9-11 | 0x080A0000  | 128K  | Free / scratch (384K)
 * --------------------------------------------------------------------- */

#define FLASH_MEM_BASE      0x08000000U
#define FLASH_MEM_SIZE      (1024U * 1024U)
#define FLASH_SECTOR_COUNT  12U

#define BOOTLOADER_BASE     0x08000000U
#define BOOTLOADER_SIZE     (64U * 1024U)

#define METADATA_BASE       0x08010000U
#define METADATA_SIZE       (64U * 1024U)
#define METADATA_SECTOR     4U

#define SLOT_A_BASE         0x08020000U   /* execution slot */
#define SLOT_A_SIZE         (256U * 1024U)
#define SLOT_A_SECTOR_FIRST 5U
#define SLOT_A_SECTOR_LAST  6U

#define SLOT_B_BASE         0x08060000U   /* staging slot */
#define SLOT_B_SIZE         (256U * 1024U)
#define SLOT_B_SECTOR_FIRST 7U
#define SLOT_B_SECTOR_LAST  8U

#define SCRATCH_BASE        0x080A0000U
#define SCRATCH_SIZE        (384U * 1024U)
#define SCRATCH_SECTOR_FIRST 9U
#define SCRATCH_SECTOR_LAST  11U

#define SANDBOX_SECTOR      9U
#define SANDBOX_BASE        0x080A0000U
#define SANDBOX_SIZE        (128U * 1024U)

/* Value of every byte in an erased sector. */
#define FLASH_ERASED_BYTE   0xFFU
#define FLASH_ERASED_WORD   0xFFFFFFFFU

typedef struct
{
    uint32_t base;
    uint32_t size;
} Flash_Sector;

/* Defined once in src/common/flash.c */
extern const Flash_Sector flash_sectors[FLASH_SECTOR_COUNT];