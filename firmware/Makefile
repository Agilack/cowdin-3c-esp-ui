##
 # @file  Makefile
 # @brief Script to compile sources and create flat binary using "make" program
 #
 # @author Saint-Genest Gwenael <gwen@agilack.fr>
 # @copyright Agilack (c) 2022
 #
 # @page License
 # Cowdin-3C-ui firmware is free software: you can redistribute it and/or
 # modify it under the terms of the GNU Lesser General Public License
 # version 3 as published by the Free Software Foundation. You should
 # have received a copy of the GNU Lesser General Public License along
 # with this program, see LICENSE.md file for more details.
 # This program is distributed WITHOUT ANY WARRANTY.
##
CROSS=arm-none-eabi-
BUILDDIR=build/
TARGET=cowdin-ui

ASRC = startup.s
SRC  = main.c hardware.c uart.c display.c

CC  = $(CROSS)gcc
OC  = $(CROSS)objcopy
OD  = $(CROSS)objdump
GDB = $(CROSS)gdb

CFLAGS  = -mcpu=cortex-m0plus -mthumb
CFLAGS += -nostdlib -Os -ffunction-sections
CFLAGS += -fno-builtin-memset -fno-builtin-memcpy
CFLAGS += -Wall -Wextra
CFLAGS += -Isrc
CFLAGS += -g

LDFLAGS  = -nostartfiles -static
LDFLAGS += -T src/linker.ld -Wl,-Map=$(TARGET).map,--cref,--gc-sections -static

AOBJ = $(patsubst %.s, build/%.o,$(ASRC))
COBJ = $(patsubst %.c, build/%.o,$(SRC))

## Directives ##################################################################

all: $(BUILDDIR) $(AOBJ) $(COBJ)
	@echo "  [LD] $(TARGET)"
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET).elf $(AOBJ) $(COBJ)
	@echo "  [OC] $(TARGET).bin"
	@$(OC) -S $(TARGET).elf -O binary $(TARGET).bin
	@echo "  [OD] $(TARGET).dis"
	@$(OD) -D $(TARGET).elf > $(TARGET).dis

clean:
	@echo "  [RM] $(TARGET).*"
	@rm -f $(TARGET).elf $(TARGET).map $(TARGET).bin $(TARGET).dis
	@echo "  [RM] Temporary object (*.o)"
	@rm -f $(BUILDDIR)*.o
	@rm -rf $(BUILDDIR)

$(BUILDDIR):
	@echo "  [MKDIR] $@"
	@mkdir $(BUILDDIR)

build/%.o : src/%.s
	@echo "  [AS] $@"
	@$(CC) $(CFLAGS) -c $< -o $@

build/%.o: src/%.c
	@echo "  [CC] $@"
	@$(CC) $(CFLAGS) -c $< -o $@

debug:
	$(GDB) --command=scripts/gdb.cfg $(TARGET).elf
