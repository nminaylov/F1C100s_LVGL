#
# Machine makefile
#

BUILDDIR = build
	
	# toolchain
TOOLCHAIN    = arm-none-eabi-
CC           = $(TOOLCHAIN)gcc
CP           = $(TOOLCHAIN)objcopy
SZ           = $(TOOLCHAIN)size
AS           = $(TOOLCHAIN)gcc -x assembler-with-cpp

# all the files will be generated with this name (main.elf, main.bin, main.hex, etc)
PROJECT_NAME = firmware

# link file
LINK_SCRIPT  = f1c100s.ld

# user specific
CSRCS = \
	src/main.c \
	src/lv_demo_widgets.c \
	src/display.c \
	src/touch.c \
	arch/machine/sys-clock.c \
	arch/machine/sys-uart.c \
	arch/machine/sys-mmu.c \
	arch/machine/exception.c \
	driver/gpio-f1c100s.c \
	driver/pwm-f1c100s.c \
	driver/f1c100s_de.c \
	driver/f1c100s_clock.c \
	lcd_lib/lcd.c \
	lcd_lib/lcd_font.c \
	lib/printf.c \
	lib/syscalls.c
	
CSRCS += \
	arch/machine/start.S \
	arch/arm32/lib/memcpy.S \
	arch/arm32/lib/memset.S \
	arch/arm32/lib/cpu/cache-v5.S \
	

# include
LIBS 		+= -lgcc -lm -lc_nano 
INCLUDES	+= -Iarch/machine/include -Iarch/arm32/include -I. -Iinc -Idriver/include -Ilib
INCLUDES	+= -Ilcd_lib/inc -Ilvgl

ELF = $(BUILDDIR)/$(PROJECT_NAME).elf
HEX = $(BUILDDIR)/$(PROJECT_NAME).hex
BIN = $(BUILDDIR)/$(PROJECT_NAME).bin

# include sub makefiles
LVGL_DIR		= .
LVGL_DIR_NAME	= lvgl
include lvgl/lvgl.mk

# defines
DEFS	 = -D__ARM32_ARCH__=5 -D__ARM926EJS__

OBJECTS = $(addprefix $(BUILDDIR)/, $(addsuffix .o, $(basename $(CSRCS))))

# Define optimisation level here
OPT = -O0

MC_FLAGS = -march=armv5te -mtune=arm926ej-s -mfloat-abi=soft -marm -mno-thumb-interwork -g -ggdb -Wall -O0 -fdata-sections -ffunction-sections -ffreestanding -std=gnu99

AS_FLAGS = $(MC_FLAGS) $(DEFS)
CFLAGS = $(MC_FLAGS) $(OPT) -fomit-frame-pointer -Wall -fverbose-asm $(DEFS)
LD_FLAGS = -nostartfiles -Xlinker --gc-sections -T$(LINK_SCRIPT) -Wl,-Map=$(BUILDDIR)/$(PROJECT_NAME).map,--cref,--no-warn-mismatch

#
# makefile rules
#
$(BIN): $(ELF)
	$(CP) -O binary $< $@
	@echo   ---------------------------------------------------
	@$(SZ) $(ELF)

$(HEX): $(ELF)
	$(CP) -O ihex $< $@

$(ELF): $(OBJECTS)
	$(CC) $(LD_FLAGS) -o $@ $(OBJECTS) $(LIBS)

$(BUILDDIR)/%.o: %.c
	"mkdir" -p $(dir $@)
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

$(BUILDDIR)/%.o: %.S
	"mkdir" -p $(dir $@)
	$(CC) -c $(AS_FLAGS) $(INCLUDES) $< -o $@

size:
	@echo   ---------------------------------------------------
	@$(SZ) $(ELF)

clean:
	rm -rf $(BUILDDIR)
