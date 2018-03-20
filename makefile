#******************************************************************************
# Makefile PS3604LF
#
TARGET := PS3604LF
ODIR := build

INCLUDES := \
	-I / \
	-I app/inc \
	-I cm4/Device/ST/STM32F4xx/Include \
	-I cm4/Include \
	-I drivers/inc \
	-I freertos/inc \
	-I lib/inc \
	-I lwip/src/include \
	-I lwip/port/include \
	-I lwip/src/include/ipv4 \
	-I lwip/src/include/lwip \
	-I lwip/port \
	-I net/inc \
	-I system/inc \
	-I task/inc

LDFILES	:= -T ldscript/STM32F407VETx_FLASH.ld
LIBS	:= lib/IQmathLib-cm4.a

CPUFLAGS := \
	-mcpu=cortex-m4 -mthumb \
	-mfloat-abi=hard \
	-mfpu=fpv4-sp-d16 \
	-DSTM32F407xx

CCFLAGS := \
	$(CPUFLAGS) \
	-std=gnu11 \
	-g3 -O2 \
	-fmessage-length=0 \
	-ffunction-sections \
	-fdata-sections \
	-fsigned-char \
	-fsingle-precision-constant \
	-Wfloat-equal \
	-Wuninitialized \
	-Wextra

LDFLAGS := \
	$(CPUFLAGS) \
	$(LDFILES) \
	-Wl,-Map="$(ODIR)/$(TARGET).map" \
	-Wl,--gc-sections \
	-Xlinker --gc-sections --specs=nano.specs

#******************************************************************************
# Toolchain
#
TOOLCHAIN_PATH := arm-none-eabi
AR		= $(TOOLCHAIN_PATH)-ar
AS		= $(TOOLCHAIN_PATH)-gcc
CC		= $(TOOLCHAIN_PATH)-gcc
CPP		= $(TOOLCHAIN_PATH)-g++
LD		= $(TOOLCHAIN_PATH)-gcc
OBJCOPY	= $(TOOLCHAIN_PATH)-objcopy
OBJDUMP	= $(TOOLCHAIN_PATH)-objdump
GDB		= $(TOOLCHAIN_PATH)-gdb
SIZE	= $(TOOLCHAIN_PATH)-size

#******************************************************************************
# C File
CSRCS := \
	$(shell find app -maxdepth 3 -type f -name "*.c") \
	$(shell find cm4 -maxdepth 7 -type f -name "*.c") \
	$(shell find drivers -maxdepth 3 -type f -name "*.c" -not -path "drivers/src/core/src/spfd54124b.c") \
	$(shell find freertos -maxdepth 3 -type f -name "*.c") \
	$(shell find lwip -maxdepth 4 -type f -name "*.c" -not -path "lwip/src/core/ipv6*") \
	$(shell find net -maxdepth 3 -type f -name "*.c") \
	$(shell find system -maxdepth 3 -type f -name "*.c") \
	$(shell find task -maxdepth 3 -type f -name "*.c")

#******************************************************************************
# CPP File
CPPSRCS :=

#******************************************************************************
# ASM File (*.S)
ASRCS := #$(shell find system -maxdepth 3 -type f -name "*.S" )

#******************************************************************************
# ASM File (*.s)
ASRCs :=

OBJODIR := $(ODIR)/obj

OBJS :=	$(CSRCS:%.c=$(OBJODIR)/%.o) \
		$(CPPSRCS:%.cpp=$(OBJODIR)/%.o) \
		$(ASRCs:%.s=$(OBJODIR)/%.o) \
		$(ASRCS:%.S=$(OBJODIR)/%.o)

DEPS :=	$(CSRCS:%.c=$(OBJODIR)/%.d) \
		$(CPPSRCS:%.cpp=$(OBJODIR)/%.d) \
		$(ASRCs:%.s=$(OBJODIR)/%.d) \
		$(ASRCS:%.S=$(OBJODIR)/%.d)

CFLAGS = $(CCFLAGS) $(INCLUDES)
DFLAGS = $(CCFLAGS) $(INCLUDES)

#******************************************************************************
# Targets
#
all: postbuild

prebuild:
	@otherFiles/vgen app/src/version.c
	@echo ' '

mainbuild: prebuild
	@$(MAKE) --no-print-directory $(TARGET).hex

postbuild: mainbuild
	@echo 'Print Size:'
	@$(SIZE) --format=berkeley "$(ODIR)/$(TARGET).elf"
	@echo ' '

clean:
	@rm -rf $(ODIR)
	@echo ' '

disasm: $(TARGET).elf
	@$(OBJDUMP) -h -S -z $(ODIR)/$(TARGET).elf > $(ODIR)/$(TARGET).S
	@echo ' '

$(TARGET).elf: $(OBJS)
	@echo [LD] $@
	@$(LD) $(LDFLAGS) $(OBJS) $(OBJS1) $(LIBS) -o $(ODIR)/$(TARGET).elf
	@echo ' '
	
$(TARGET).hex: $(TARGET).elf
	@echo [OBJCOPY] $@
	@$(OBJCOPY) -O ihex $(ODIR)/$(TARGET).elf $(ODIR)/$(TARGET).hex
	@echo ' '

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),disasm)
ifdef DEPS
sinclude $(DEPS)
endif
endif
endif

$(OBJODIR)/%.o: %.c
	@echo [CC] $<
	@mkdir -p $(dir $@)
	@$(CC) -MT $@ -MMD -MP -MF $(OBJODIR)/$*.Td $(CFLAGS) -c -o $@ $<
	@mv -f $(OBJODIR)/$*.Td $(OBJODIR)/$*.d && touch $@

$(OBJODIR)/%.o: %.cpp
	@echo [CPP] $<
	@mkdir -p $(dir $@)
	@$(CC) -MT $@ -MMD -MP -MF $(OBJODIR)/$*.Td $(CFLAGS) -c -o $@ $<
	@mv -f $(OBJODIR)/$*.Td $(OBJODIR)/$*.d && touch $@

$(OBJODIR)/%.o: %.s
	@echo [AS] $<
	@mkdir -p $(dir $@)
	@$(AS) $(CFLAGS) -M -o $(OBJODIR)/$*.d $<
	@$(AS) $(CFLAGS) -MMD -MP -MF $(OBJODIR)/$*.d -MT$@ -c -o $@ $<

$(OBJODIR)/%.o: %.S
	@echo [AS] $<
	@mkdir -p $(dir $@)
	@$(AS) $(CFLAGS) -M -o $(OBJODIR)/$*.d $<
	@$(AS) $(CFLAGS) -MMD -MP -MF $(OBJODIR)/$*.d -MT$@ -c -o $@ $<
