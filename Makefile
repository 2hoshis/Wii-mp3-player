# Minimal Wii homebrew Makefile for devkitPro/libogc.

TARGET := hello-world
BUILD := build
SOURCES := source

DEVKITPRO ?= /opt/devkitpro
DEVKITPPC ?= $(DEVKITPRO)/devkitPPC
LIBOGC := $(DEVKITPRO)/libogc

CC := $(DEVKITPPC)/bin/powerpc-eabi-gcc
ELF2DOL := $(DEVKITPRO)/tools/bin/elf2dol

MACHDEP := -DGEKKO -mrvl -mcpu=750 -meabi -mhard-float
CFLAGS := -g -O2 -Wall $(MACHDEP) -I$(LIBOGC)/include
LDFLAGS := -g $(MACHDEP) -L$(LIBOGC)/lib/wii
LIBS := -lfat -lwiiuse -lbte -logc -lm

CFILES := $(wildcard $(SOURCES)/*.c)
OFILES := $(patsubst $(SOURCES)/%.c,$(BUILD)/%.o,$(CFILES))

.PHONY: all clean

all: $(TARGET).dol

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%.o: $(SOURCES)/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OFILES)
	$(CC) $(LDFLAGS) $^ $(LIBS) -o $@

$(TARGET).dol: $(TARGET).elf
	$(ELF2DOL) $< $@

clean:
	rm -rf $(BUILD) $(TARGET).elf $(TARGET).dol
