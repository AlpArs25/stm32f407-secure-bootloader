# stm32f407-bare-metal

A from-scratch bare-metal firmware project for the **STM32F407G-DISC1**
(STM32F407VG, Cortex-M4F). No vendor HAL, no CubeMX, no CMSIS package —
startup code, device header, and drivers are written by hand.

Built as a learning project, not maximum feature coverage.

## Status

Work in progress. Nothing has been flashed to hardware yet.

| Component | State |
|---|---|
| Startup code + vector table | Written, untested |
| Linker scripts (bootloader / app) | Written, untested |
| Device header (`stm32f407xx.h`) | Partial — registers added as needed |
| GPIO driver | Init only; no read/write/toggle |
| USART driver | Init only; `usart_write`/`usart_read` are stubs |
| RCC | `hal_rcc_get_pclk1()` only; no PLL setup |
| NVIC | Enable/disable written, not wired up |
| Build system | Not written |

Runs on the default 16 MHz HSI. The PLL is deliberately not configured yet.

## Layout

```
startup_boot.c    Vector table, Reset_Handler (.data copy, .bss zero)
stm32f407xx.h/.c  Device header: register layouts, base addresses, bit defs
core_cm4.h/.c     ARM core peripherals: NVIC, SCB
hal_common.h      HAL_Status enum, shared across all drivers
gpio.h/.c         GPIO driver
usart.h/.c        USART driver
rcc.h/.c          Clock tree queries
main.c            Application entry
bootloader.ld     Linker script: 64K at 0x08000000 (sectors 0-3)
app.ld            Linker script: 128K at 0x08020000 (sector 5)
```

## Design rules

These are the boundaries the code is organised around. Breaking them is how
the file layout rots.

**1. Four layers, by scope.**

| Layer | Scope | Example |
|---|---|---|
| `stm32f407xx.h` | Chip | `GPIOA_BASE`, `RCC_APB1ENR_USART2EN` |
| `core_cm4.h` | ARM core | `NVIC`, `SCB_VTOR` |
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
erase its neighbour. F407 sectors: 0-3 are 16K, 4 is 64K, 5-11 are 128K.

## Build

Not yet implemented. Requires `arm-none-eabi-gcc`.

On Fedora:

```
sudo dnf install arm-none-eabi-gcc-cs arm-none-eabi-newlib arm-none-eabi-binutils-cs
```

Planned flags: `-mcpu=cortex-m4 -mthumb -nostdlib -nostartfiles`
plus `-Wall -Wextra -Wmissing-prototypes`.

No FPU. `-mfloat-abi=soft`; the baud rate calculation is integer-only by
design.

## References

- RM0090 — STM32F405/407/415/417 reference manual (register maps)
- STM32F407VG datasheet (alternate function tables, pinout)
- UM1472 — STM32F4 Discovery board user manual (board wiring)
- ARMv7-M Architecture Reference Manual (core exceptions, NVIC, VTOR)
