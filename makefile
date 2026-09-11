CROSS_COMPILE := arm-none-eabi-
CC := $(CROSS_COMPILE)gcc
OBJCOPY := $(CROSS_COMPILE)objcopy
SIZE := $(CROSS_COMPILE)size

MCU := -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard

CFLAGS := $(MCU) -g -ggdb -Wall -Wextra -Wmissing-prototypes -Werror \
          -ffunction-sections -fdata-sections -MMD -MP -Iinclude

# Link via gcc (driver), not raw ld — see explanation above.
# -nostartfiles: don't pull in gcc's default crt0/startup, we provide our own (startup_*.o)
# -specs=nosys.specs: weak stub syscalls (_write/_read/_sbrk/...), overridden by our syscalls.o
# -specs=nano.specs: use newlib-nano (smaller libc)
LDFLAGS := $(MCU) -nostartfiles -specs=nosys.specs -specs=nano.specs \
           -Wl,--gc-sections

BUILD      := build
COMMON_SRC := $(wildcard src/common/*.c)
BL_SRC     := $(wildcard src/bl/*.c)  $(COMMON_SRC)
APP_SRC    := $(wildcard src/app/*.c) $(COMMON_SRC)
BL_OBJ     := $(patsubst %.c,$(BUILD)/bl/%.o,$(BL_SRC))
APP_OBJ    := $(patsubst %.c,$(BUILD)/app/%.o,$(APP_SRC))

.PHONY: all clean size flash

all: $(BUILD)/image.bin size

flash: $(BUILD)/image.bin
	st-flash write $(BUILD)/image.bin 0x08000000

size: $(BUILD)/app.elf $(BUILD)/bootloader.elf
	@echo "----- bootloader.elf -----"
	@$(SIZE) $(BUILD)/bootloader.elf
	@echo "----- app.elf -----"
	@$(SIZE) $(BUILD)/app.elf
	@echo "----- output files -----"
	@ls -la $(BUILD)/*.bin $(BUILD)/*.elf

$(BUILD)/image.bin: $(BUILD)/app.bin $(BUILD)/bootloader.bin
	cat $(BUILD)/bootloader.bin $(BUILD)/app.bin > $@

$(BUILD)/app.bin: $(BUILD)/app.elf
	$(OBJCOPY) -O binary $< $@

$(BUILD)/bootloader.bin: $(BUILD)/bootloader.elf
	$(OBJCOPY) -O binary --pad-to=0x08020000 --gap-fill=0xff $< $@

$(BUILD)/bootloader.elf: $(BL_OBJ)
	$(CC) $(LDFLAGS) $^ -o $@ -Wl,-Map=$(BUILD)/bootloader.map -T ld/bootloader.ld

$(BUILD)/app.elf: $(APP_OBJ)
	$(CC) $(LDFLAGS) $^ -o $@ -Wl,-Map=$(BUILD)/app.map -T ld/app.ld

clean:
	rm -rf $(BUILD)

-include $(wildcard $(BUILD)/bl/src/*/*.d) $(wildcard $(BUILD)/app/src/*/*.d)

$(BUILD)/bl/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -DIMAGE_BOOTLOADER -c $< -o $@

$(BUILD)/app/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -DIMAGE_APP -c $< -o $@