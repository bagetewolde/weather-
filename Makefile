MCU       = atmega328p
F_CPU     = 16000000UL

CC        = avr-gcc
OBJCOPY   = avr-objcopy
SIZE      = avr-size
AVRDUDE   = avrdude

TARGET    = weather_station

SRC_DIR   = src
INC_DIR   = include
BUILD_DIR = build

SOURCES   = $(wildcard $(SRC_DIR)/*.c)
OBJECTS   = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

ELF       = $(BUILD_DIR)/$(TARGET).elf
HEX       = $(BUILD_DIR)/$(TARGET).hex

AVRDUDE_PROGRAMMER ?= arduino
AVRDUDE_PORT       ?= COM8
AVRDUDE_BAUD       ?= 115200

CFLAGS = -mmcu=$(MCU) \
         -DF_CPU=$(F_CPU) \
         -I$(INC_DIR) \
         -std=gnu11 \
         -Os \
         -Wall \
         -Wextra \
         -Wshadow \
         -Wstrict-prototypes \
         -ffunction-sections \
         -fdata-sections

LDFLAGS = -mmcu=$(MCU) -Wl,--gc-sections

.PHONY: all clean size upload

all: $(HEX) size

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(ELF): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex -R .eeprom $< $@

size: $(ELF)
	$(SIZE) $(ELF)

upload: $(HEX)
	$(AVRDUDE) -p $(MCU) \
	    -c $(AVRDUDE_PROGRAMMER) \
	    -P $(AVRDUDE_PORT) \
	    -b $(AVRDUDE_BAUD) \
	    -D -U flash:w:$(HEX):i

clean:
	rm -rf $(BUILD_DIR)