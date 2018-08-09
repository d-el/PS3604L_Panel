#******************************************************************************
# Makefile PS3604LF
#
TARGET := PS3604LF
ODIR := build

# A simple variant is to prefix commands with $(Q) - that's useful
# for commands that shall be hidden in non-verbose mode.
#
#	$(Q)ln $@ :<
#
# To put more focus on warnings, be less verbose as default
# Use 'make V=1' to see the full commands
ifeq ("$(origin V)", "command line")
	BUILD_VERBOSE = $(V)
else
	BUILD_VERBOSE = 0
endif

ifeq ($(BUILD_VERBOSE),1)
	Q =
else
	Q = @
endif

# Do not print "Entering directory ...",
# but we want to display it when entering to the output directory
# so that IDEs/editors are able to understand relative filenames.
MAKEFLAGS += --no-print-directory

# Cross compiling and selecting different set of gcc/bin-utils
# ---------------------------------------------------------------------------
#
# CROSS_COMPILE specify the prefix used for all executables used
# during compilation. Only gcc and related bin-utils executables
# are prefixed with $(CROSS_COMPILE).
# CROSS_COMPILE can be set on the command line
# make CROSS_COMPILE=ia64-linux-
# Alternatively CROSS_COMPILE can be set in the environment.
# Default value for CROSS_COMPILE is arm-none-eabi-
CROSS_COMPILE ?= arm-none-eabi-

# Make variables (CC, etc...)
AS		= $(CROSS_COMPILE)gcc
LD		= $(CROSS_COMPILE)gcc
CC		= $(CROSS_COMPILE)gcc
CPP		= $(CC) -E
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm
STRIP	= $(CROSS_COMPILE)strip
OBJCOPY	= $(CROSS_COMPILE)objcopy
OBJDUMP	= $(CROSS_COMPILE)objdump
SIZE	= $(CROSS_COMPILE)size

INCLUDES := \
	-I app/inc \
	-I cm4/Device/ST/STM32F4xx/Include \
	-I cm4/Include \
	-I drivers/inc \
	-I freertos/inc \
	-I lib/inc \
	-I lwip/src/include/lwip \
	-I lwip/src/include \
	-I lwip/port/include \
	-I lwip/port \
	-I net/inc \
	-I system/inc \
	-I task/inc \
	-I utils/inc

LDFILES	:= -T ldscript/STM32F407VETx_FLASH.ld
LIBS	:= lib/IQmathLib-cm4.a

CPUFLAGS := \
	-mcpu=cortex-m4 -mthumb \
	-mfloat-abi=hard \
	-mfpu=fpv4-sp-d16 \
	-DSTM32F407xx

COMMONFLAGS := \
	-g3 -O2 \
	-fmessage-length=0 \
	-ffunction-sections \
	-fdata-sections \
	-fsigned-char \
	-fsingle-precision-constant \
	-Wfloat-equal \
	-Wuninitialized \
	-Wextra

CCFLAGS := \
	$(CPUFLAGS) \
	$(COMMONFLAGS) \
	-std=gnu11

CPPFLAGS := \
	$(CPUFLAGS) \
	$(COMMONFLAGS) \
	-std=c++1y \
	-fno-rtti \
	-fno-exceptions

LDFLAGS := \
	$(CPUFLAGS) \
	$(LDFILES) \
	-Wl,-Map="$(ODIR)/$(TARGET).map" \
	-Wl,--gc-sections \
	-Xlinker --gc-sections --specs=nano.specs \
	-Wl,--print-memory-usage \
	-Wl,--undefined=uxTopUsedPriority

#******************************************************************************
# C File
CSRCS := \
	$(shell find app -maxdepth 3 -type f -name "*.c") \
	$(shell find cm4 -maxdepth 7 -type f -name "*.c") \
	$(shell find drivers -maxdepth 3 -type f -name "*.c") \
	$(shell find freertos -maxdepth 3 -type f -name "*.c") \
	$(shell find lwip -maxdepth 4 -path lwip/src/core/ipv6 -prune -o -type f -name '*.c' -print) \
	$(shell find net -maxdepth 3 -type f -name "*.c") \
	$(shell find system -maxdepth 3 -type f -name "*.c") \
	$(shell find task -maxdepth 3 -type f -name "*.c") \
	$(shell find utils -maxdepth 3 -type f -name "*.c")

#******************************************************************************
# CPP File
CPPSRCS :=

#******************************************************************************
# ASM File (*.S)
ASRCS :=

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
CPFLAGS = $(CPPFLAGS) $(INCLUDES)

#******************************************************************************
# Targets
#
PHONY += all
all: postbuild

PHONY += prebuild
prebuild:
	$(Q)scripts/vgen.sh app/src/version.c
	@echo ' '

mainbuild: prebuild
	$(Q)$(MAKE) robjcopy

postbuild: mainbuild
	@echo 'Print Size:'
	$(Q)$(SIZE) --format=berkeley "$(ODIR)/$(TARGET).elf"
	@echo ' '

elf: $(OBJS)
	@echo [LD] $@
	$(Q)$(LD) $(LDFLAGS) $(OBJS) $(LIBS) -o $(ODIR)/$(TARGET).elf
	@echo ' '
	
robjcopy: elf
	@echo [OBJCOPY] $@
	$(Q)$(OBJCOPY) -O ihex $(ODIR)/$(TARGET).elf $(ODIR)/$(TARGET).hex
	@echo ' '

PHONY += clean
clean:
	$(Q)rm -rf $(ODIR)
	@echo ' '

disasm: elf
	$(Q)$(OBJDUMP) -h -S -z $(ODIR)/$(TARGET).elf > $(ODIR)/$(TARGET).S
	@echo ' '

PHONY += listc
listc:
	@echo $(CSRCS) $(CPPSRCS) $(CASRCS)

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),disasm)
ifdef DEPS
sinclude $(DEPS)
endif
endif
endif

$(OBJODIR)/%.o: %.c
	@echo [CC] $<
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CC) -MT $@ -MMD -MP -MF $(OBJODIR)/$*.Td $(CFLAGS) -c -o $@ $<
	$(Q)mv -f $(OBJODIR)/$*.Td $(OBJODIR)/$*.d && touch $@

$(OBJODIR)/%.o: %.cpp
	@echo [CPP] $<
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CC) -MT $@ -MMD -MP -MF $(OBJODIR)/$*.Td $(CPFLAGS) -c -o $@ $<
	$(Q)mv -f $(OBJODIR)/$*.Td $(OBJODIR)/$*.d && touch $@

$(OBJODIR)/%.o: %.s
	@echo [AS] $<
	$(Q)mkdir -p $(dir $@)
	$(Q)$(AS) $(CFLAGS) -M -o $(OBJODIR)/$*.d $<
	$(Q)$(AS) $(CFLAGS) -MMD -MP -MF $(OBJODIR)/$*.d -MT$@ -c -o $@ $<

$(OBJODIR)/%.o: %.S
	@echo [AS] $<
	$(Q)mkdir -p $(dir $@)
	$(Q)$(AS) $(CFLAGS) -M -o $(OBJODIR)/$*.d $<
	$(Q)$(AS) $(CFLAGS) -MMD -MP -MF $(OBJODIR)/$*.d -MT$@ -c -o $@ $<

# Declare the contents of the .PHONY variable as phony.  We keep that
# information in a variable so we can use it in if_changed and friends.
.PHONY: $(PHONY)