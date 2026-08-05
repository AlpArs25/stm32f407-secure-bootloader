CROSS_COMPILE := arm-none-eabi-
CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld
OBJCOPY := $(CROSS_COMPILE)objcopy

CFLAGS := -mcpu=cortex-m4 -mthumb -g -ggdb -Wall -Wextra -Wmissing-prototypes -ffunction-sections -fdata-sections
LDFLAGS := --gc-sections -nostdlib

OBJS := gpio.o core_cm4.o rcc.o stm32f407xx.o usart.o

.PHONY: all clean

all: image.bin

image.bin: app.bin bootloader.bin
	cat bootloader.bin app.bin > image.bin

app.bin: app.elf
	$(OBJCOPY) -O binary $< $@

bootloader.bin: bootloader.elf
	$(OBJCOPY) -O binary --pad-to=0x08020000 --gap-fill=0xff $< $@

bootloader.elf: startup_bl.o
	$(LD) $(LDFLAGS) startup_bl.o -o $@ -Map=bootloader.map -T bootloader.ld

app.elf: $(OBJS) startup_app.o main.o
	$(LD) $(LDFLAGS) $^ -o $@ -Map=app.map -T app.ld

clean:
	rm -f *.o *.bin *.elf *.map

%.o: %.s
	$(CC) $(CFLAGS) -c $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@
