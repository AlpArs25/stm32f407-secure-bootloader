# stm32f407-secure-bootloader

A from-scratch A/B firmware update system for the **STM32F407G-DISC1**
(STM32F407VG, Cortex-M4F): custom bootloader, signed images, watchdog
rollback. No vendor HAL, no CubeMX, no CMSIS package — startup code, device
header, core header, and drivers are written by hand.

Built as a learning project. The goal is understanding every instruction
between reset and `main()`, not maximum feature coverage.

## Status

**Stages 1–2 verified on hardware.** The bootloader boots from sector 0,
blinks the orange LED, relocates the vector table (`VTOR`), loads the app's
stack pointer, and branches to the app at `0x08020000`, which blinks blue.

| Component | State |
|---|---|
| Startup code + vector table (both images) | **Working on hardware** |
| Bootloader → app jump (VTOR + MSP + `bx`) | **Working on hardware** |
| Linker scripts (bootloader / app) | Working |
| SysTick + `delay_ms` | Working |
| LED driver | Working |
| Fault handler (red-LED blink loop) | Written |
| GPIO driver | Init + used by LED/USART paths |
| USART driver | Written, not yet exercised on hardware |
| `printf` retarget (`_write` stub) | Stub present; link support pending |
| RCC | `PCLK1` query only; no PLL setup |
| Build system (make) | Working — both images + combined factory image + size report |
| Flash driver, transfer protocol, metadata, signatures | Not started (stages 3–5) |

Runs on the default 16 MHz HSI. The PLL is deliberately not configured yet.

Code is built soft-float. The FPU is still enabled via CPACR in
`Reset_Handler` so a later switch to hard-float cannot introduce a usage
fault.

## Flash memory map

The F407's 1 MB flash has non-uniform sectors: 4×16K, 1×64K, 7×128K.
Every region starts and ends on a sector boundary (design rule 6).

| Region | Sectors | Address | Size |
|---|---|---|---|
| Bootloader | 0–3 | `0x08000000` | 64K |
| Metadata | 4 | `0x08010000` | 64K |
| Slot A (execution slot) | 5–6 | `0x08020000` | 256K |
| Slot B (staging slot) | 7–8 | `0x08060000` | 256K |
| Free / scratch | 9–11 | `0x080A0000` | 384K |

RAM: bootloader and app both link against SRAM at `0x20000000`. The regions
overlap on purpose — the bootloader has finished executing before the app
starts, so they never coexist.

## Boot flow

Current (stages 1–2):

1. Bootloader initialises SysTick and LEDs, blinks orange.
2. Disables interrupts, sets `VTOR` to the app's vector table at
   `0x08020000`.
3. Loads MSP from the app's word 0 and branches (`bx`) to the app's reset
   vector (word 1) in a single asm block, so no stack access can occur
   between the stack switch and the jump.
4. App re-initialises SysTick and blinks blue.

Target (stages 3–5): header validation (magic, length, CRC, Ed25519
signature), copy from staging slot to execution slot, `PENDING` boot under
an armed watchdog with automatic rollback.

## Layout

```
startup_bl.c      Bootloader image: vector table + Reset_Handler
startup_app.c     App image: vector table + Reset_Handler
bl_main.c         Bootloader logic: blink, VTOR, MSP, jump
app.main.c        Application entry: blink
stm32f407xx.h/.c  Device header: register layouts, base addresses, bit defs
core_cm4.h/.c     ARM core: NVIC, VTOR, CPACR, SysTick, barrier intrinsics
hal_common.h      HAL_Status enum + status-propagation macros
gpio.h/.c         GPIO driver
usart.h/.c        USART driver (USART2, polled)
led.h/.c          Board LEDs (PD12–PD15)
fault.c/.h        Terminal fault handler: red LED blink loop
syscalls.c        Newlib retarget: _write → USART2
rcc.h/.c          Clock tree queries
bootloader.ld     Linker script: 64K at 0x08000000 (sectors 0–3)
app.ld            Linker script: at 0x08020000 (slot A)
makefile          Builds bootloader.bin, app.bin, combined image.bin
```

## Design rules

These are the boundaries the code is organised around. Breaking them is how
the file layout rots.

**1. Four layers, by scope.**

| Layer | Scope | Example |
|---|---|---|
| `stm32f407xx.h` | Chip | `GPIOA_BASE`, `RCC_APB1ENR_USART2EN` |
| `core_cm4.h` | ARM core | `NVIC`, `VTOR`, `SYSTICK` |
| `gpio.c`, `usart.c` | Chip family | `gpio_init()`, `usart_init()` |
| `led.c`, pin constants | Board wiring | `led_toggle()`, `USART2_TX_PIN` |

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
`HAL_OK` is 0. Errors propagate with `HAL_TRY` (early return) or halt with
`HAL_CHECK` (fault handler). Unrecognised peripheral instances return
`HAL_ERROR`, never a silent success.

**5. Config structs, not parameter lists.**
`GPIO_Config` / `USART_Config` are passed by pointer. Defaults come from
`GPIO_CONFIG_DEFAULT`; callers copy it and override the fields they need.

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

**Single asm block for the stack switch + jump.** Once MSP is rewritten,
any compiler-generated access to the old stack reads garbage. Loading both
the new stack pointer and the entry address as register operands of one
`asm volatile("msr msp, %0 \n bx %1")` guarantees nothing touches the stack
in between.

## Build and flash

Requires `arm-none-eabi-gcc` and `stlink` tools. On Fedora:

```
sudo dnf install arm-none-eabi-gcc-cs arm-none-eabi-newlib arm-none-eabi-binutils-cs stlink
make        # builds bootloader.bin, app.bin, image.bin + size report
make flash  # st-flash write image.bin 0x08000000
```

`image.bin` is the factory image: bootloader padded to 128K with `0xFF`
(erased-flash value, so the metadata sector region stays blank) followed by
the app, flashable as one piece at `0x08000000`.

## Serial console

USART2 on PA2 (TX) / PA3 (RX), 115200 8N1, via an external USB–TTL cable
(the onboard ST-LINK exposes no VCP on this board). Connect GND–GND,
cable TX → PA3, cable RX → PA2; leave the cable's VCC unconnected and power
the board from the ST-LINK USB.

```
tio /dev/ttyUSB0 -b 115200
```

## Roadmap

- [x] Stage 1 — bare-metal skeleton: startup, linker scripts, drivers, blink
- [x] Stage 2 — split bootloader/app images, VTOR relocation, verified jump
- [ ] Stage 2.5 — printf over USART2 (newlib-nano link, `_sbrk`, banners)
- [ ] Stage 3 — internal flash driver + UART transfer protocol into slot B
- [ ] Stage 4 — metadata journal, PENDING/CONFIRMED states, IWDG rollback
- [ ] Stage 5 — Ed25519 image signatures
- [ ] Stage 6 — Python host tool, CI, docs

## References

- RM0090 — STM32F405/407/415/417 reference manual (register maps)
- STM32F407VG datasheet (alternate function tables, pinout)
- UM1472 — STM32F4 Discovery board user manual (board wiring)
- ARMv7-M Architecture Reference Manual (core exceptions, NVIC, VTOR)
