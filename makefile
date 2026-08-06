CROSS_COMPILE := arm-none-eabi-
CC := $(CROSS_COMPILE)gcc
OBJCOPY := $(CROSS_COMPILE)objcopy
SIZE := $(CROSS_COMPILE)size

MCU := -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard

CFLAGS := $(MCU) -g -ggdb -Wall -Wextra -Wmissing-prototypes -Werror \
          -ffunction-sections -fdata-sections -MMD -MP

# Link via gcc (driver), not raw ld — see explanation above.
# -nostartfiles: don't pull in gcc's default crt0/startup, we provide our own (startup_*.o)
# -specs=nosys.specs: weak stub syscalls (_write/_read/_sbrk/...), overridden by our syscalls.o
# -specs=nano.specs: use newlib-nano (smaller libc)
LDFLAGS := $(MCU) -nostartfiles -specs=nosys.specs -specs=nano.specs \
           -Wl,--gc-sections

OBJS := gpio.o core_cm4.o rcc.o stm32f407xx.o usart.o syscalls.o fault.o led.o

.PHONY: all clean size flash

all: image.bin size

flash: image.bin
	st-flash write image.bin 0x08000000

size: app.elf bootloader.elf
	@echo "----- bootloader.elf -----"
	@$(SIZE) bootloader.elf
	@echo "----- app.elf -----"
	@$(SIZE) app.elf
	@echo "----- output files -----"
	@ls -la *.bin *.elf

image.bin: app.bin bootloader.bin
	cat bootloader.bin app.bin > image.bin

app.bin: app.elf
	$(OBJCOPY) -O binary $< $@

bootloader.bin: bootloader.elf
	$(OBJCOPY) -O binary --pad-to=0x08020000 --gap-fill=0xff $< $@

bootloader.elf: startup_bl.o bl_main.o led.o gpio.o core_cm4.o stm32f407xx.o
	$(CC) $(LDFLAGS) $^ -o $@ -Wl,-Map=bootloader.map -T bootloader.ld

app.elf: $(OBJS) startup_app.o app_main.o
	$(CC) $(LDFLAGS) $^ -o $@ -Wl,-Map=app.map -T app.ld

clean:
	rm -f *.o *.bin *.elf *.map *.d

-include $(wildcard *.d)

%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@