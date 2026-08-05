# stm32f407-secure-bootloader

A from-scratch A/B firmware update system for the **STM32F407G-DISC1**
(STM32F407VG, Cortex-M4F): custom bootloader, signed images, watchdog
rollback. No vendor HAL, no CubeMX, no CMSIS package — startup code, device
header, core header, and drivers are written by hand.

Built as a learning project. The goal is understanding every instruction
between reset and `main()`, not maximum feature coverage.

## Status

Work in progress — stage 1 of 6. Nothing has been flashed to hardware yet.

| Component | State |
|---|---|
| Startup code + vector table (both images) | Written, untested |
| Linker scripts (bootloader / app) | Written, untested |
| Bootloader jump to app (VTOR + MSP + branch) | Written, untested |
| Device header (`stm32f407xx.h`) | Partial — registers added as needed |
| Core header (`core_cm4.h`) — NVIC, VTOR, CPACR, barriers | Written, untested |
| GPIO driver | Init only; no read/write/toggle yet |
| USART driver | Init only; `usart_write`/`usart_read` are stubs |
| RCC | `hal_rcc_get_pclk1()` only; no PLL setup |
| Build system (make) | Working — builds both images + combined factory image |
| Flash transfer protocol, metadata, signatures | Not started (stages 3–5) |

Runs on the default 16 MHz HSI. The PLL is deliberately not configured yet.

Code is built soft-float (no `-mfloat-abi` flag; soft is the default). The
FPU is still enabled via CPACR in `Reset_Handler` so that a later switch to
hard-float cannot introduce a usage fault.

## Flash memory map

The F407's 1 MB flash has non-uniform sectors: 4×16K, 1×64K, 7×128K.
Every region below starts and ends on a sector boundary (see design rule 6).

| Region | Sectors | Address | Size |
|---|---|---|---|
| Bootloader | 0–3 | `0x08000000` | 64K |
| Metadata | 4 | `0x08010000` | 64K |
| Slot A (execution slot) | 5–6 | `0x08020000` | 256K |
| Slot B (staging slot) | 7–8 | `0x08060000` | 256K |
| Free | 9–11 | `0x080A0000` | 384K |

RAM: bootloader and app both link against SRAM at `0x20000000`. The regions
overlap on purpose — the bootloader has finished executing before the app
starts, so they never coexist.

## Boot flow (target design)

1. Bootloader validates the image header in the execution slot
   (magic, length, CRC — later: Ed25519 signature).
2. If a newer image is staged in slot B, it is copied into the execution
   slot after validation.
3. Bootloader disables interrupts, de-inits peripherals, sets `VTOR` to the
   app's vector table, loads MSP from the app's word 0, branches to the app's
   reset vector (word 1).
4. New images boot in `PENDING` state under an armed watchdog; the app must
   confirm within a deadline or the bootloader rolls back (stage 4).

## Layout

```
startup_bl.c      Bootloader image: vector table + Reset_Handler
startup_app.c     App image: vector table + Reset_Handler
stm32f407xx.h/.c  Device header: register layouts, base addresses, bit defs
core_cm4.h/.c     ARM core: NVIC, SCB (VTOR), CPACR, barrier intrinsics
hal_common.h      HAL_Status enum, shared across all drivers
gpio.h/.c         GPIO driver
usart.h/.c        USART driver
rcc.h/.c          Clock tree queries
main.c            Application entry
bootloader.ld     Linker script: 64K at 0x08000000 (sectors 0–3)
app.ld            Linker script: at 0x08020000 (slot A)
makefile          Builds bootloader.bin, app.bin, and combined image.bin
```

## Design rules

These are the boundaries the code is organised around. Breaking them is how
the file layout rots.

**1. Four layers, by scope.**

| Layer | Scope | Example |
|---|---|---|
| `stm32f407xx.h` | Chip | `GPIOA_BASE`, `RCC_APB1ENR_USART2EN` |
| `core_cm4.h` | ARM core | `NVIC`, `VTOR` |
| `gpio.c`, `usart.c` | Chip family | `gpio_init()`, `usart_init()` |
| `usart.h` constants | Board wiring | `USART2_TX_PIN = 2` |

Two tests decide where something goes:

- *Would this be true on a different board with the same chip?*
  Yes -> device header. No -> board/driver layer.
- *Would this be true for a different application on this board?*
  Yes -> board layer. No -> application.

**2. Drivers take the peripheral instance as a parameter.**
`gpio_init(GPIO_TypeDef *port, ...)`, not a hardcoded `GPIOA` inside.
Pin numbers and ports are supplied by the caller, never derived.

**3. Headers declare, `.c` files define.**
No variable definitions in headers. Shared constants use `extern` in the
header plus one definition in a `.c`. `static` at file scope means private.

**4. Every driver function returns `HAL_Status`.**
`HAL_OK` is 0. Unrecognised peripheral instances return `HAL_ERROR`, never
a silent success.

**5. Config structs, not parameter lists.**
`GPIO_Config` / `USART_Config` are passed by pointer. Defaults come from
`GPIO_Config_Default`; callers copy it and override the fields they need.

**6. Flash regions are sector-aligned.**
Flash erases a whole sector at a time. A region that ends mid-sector will
erase its neighbour. F407 sectors: 0–3 are 16K, 4 is 64K, 5–11 are 128K.

**7. Write-1-to-clear registers get `=`, never `|=`.**
Registers like `NVIC->ICER` return live state on read; a read-modify-write
acts on every set bit, not just yours. Plain assignment only.

## Design decisions

**Copy-to-execution-slot, not one-build-per-slot.** The F407 has no
dual-bank flash, so slot A and slot B live at different addresses and the
app cannot run unmodified from both. Two options:

- *Build twice, execute in place* — instant boot, but two binaries per
  release, and the signature covers a slot-specific image.
- *Copy to a fixed execution slot* (chosen) — one binary, one signature,
  the host tool never needs to know which slot it is writing; the cost is a
  copy step at boot and a boot time that grows with image size.

For a single-developer project the halved build/sign/release matrix wins.

## Build

Requires `arm-none-eabi-gcc`. On Fedora:

```
sudo dnf install arm-none-eabi-gcc-cs arm-none-eabi-newlib arm-none-eabi-binutils-cs
make
```

Produces:

- `bootloader.bin` — padded to 128K with `0xFF` (erased-flash value, so the
  metadata sector region stays blank)
- `app.bin`
- `image.bin` — bootloader + app concatenated; flashable as one piece at
  `0x08000000`

Flash (once hardware bring-up starts):

```
st-flash write image.bin 0x08000000
```

## References

- RM0090 — STM32F405/407/415/417 reference manual (register maps)
- PM0059 / RM0090 ch.3 — flash programming (sector erase, PSIZE)
- STM32F407VG datasheet (alternate function tables, pinout)
- UM1472 — STM32F4 Discovery board user manual (board wiring)
- ARMv7-M Architecture Reference Manual (core exceptions, NVIC, VTOR)
