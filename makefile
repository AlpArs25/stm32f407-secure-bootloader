CROSS_COMPILE := arm-none-eabi-
CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld
OBJCOPY := $(CROSS_COMPILE)objcopy
SIZE := $(CROSS_COMPILE)size

CFLAGS := -mcpu=cortex-m4 -mthumb -g -ggdb -Wall -Wextra -Wmissing-prototypes -Werror -ffunction-sections -fdata-sections -MMD -MP
LDFLAGS := --gc-sections -nostdlib

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
	$(LD) $(LDFLAGS) $^ -o $@ -Map=bootloader.map -T bootloader.ld

app.elf: $(OBJS) startup_app.o app_main.o
	$(LD) $(LDFLAGS) $^ -o $@ -Map=app.map -T app.ld

clean:
	rm -f *.o *.bin *.elf *.map

%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
