# vim: set noexpandtab tabstop=4 shiftwidth=4:

#---------------------------------------
# Variable
#---------------------------------------
# Target
TARGET			= loader

TARGET_AXF		= $(TARGET).axf
TARGET_MAP		= $(TARGET).map
TARGET_SYM		= $(TARGET).sym
TARGET_DUMP		= $(TARGET).dump
TARGET_BIN		= $(TARGET).bin
TARGET_SREC		= $(TARGET).srec

# Source
LDS				= loader.ld

ASMS			=
ASMS			+= startup.S

SRCS			=
SRCS			+= loader.c
SRCS			+= loader_cached.c

LDS_GEN			= $(LDS).gen
DPNS			= $(ASMS:.S=.d) $(SRCS:.c=.d)
OBJS			= $(ASMS:.S=.o) $(SRCS:.c=.o)

# Directory
INCLUDE_DIRS	= .
SRC_DIRS		= .

# Command
PREFIX			= aarch64-elf-

GCC				= $(PREFIX)gcc
AR				= $(PREFIX)ar
LD				= $(PREFIX)ld
NM				= $(PREFIX)nm
OBJCOPY			= $(PREFIX)objcopy
OBJDUMP			= $(PREFIX)objdump
SIZE			= $(PREFIX)size

CFLAGS			=
CFLAGS			+= $(addprefix -I, $(INCLUDE_DIRS))
#CFLAGS			+= -mh -mrelax -mint32 -O2
CFLAGS			+= -Wall
CFLAGS			+= -mgeneral-regs-only
CFLAGS			+= -O0 -gdwarf-2

ASFLAGS			=

LDFLAGS			=
LDFLAGS			+= -T $(LDS_GEN)
LDFLAGS			+= -nostdlib
LDFLAGS			+= -Wl,--build-id=none

RM				= rm -f

#---------------------------------------
# Special Variable
#---------------------------------------
vpath %.h	= $(INCLUDE_DIRS)
vpath %.S	= $(SRC_DIRS)
vpath %.c	= $(SRC_DIRS)

.PHONY: all map sym dump bin srec clean
.SUFFIXES: .h .S .c .d .o

#---------------------------------------
# Command
#---------------------------------------
#all: axf map sym dump bin
all: bin

axf: $(TARGET_AXF)

map: $(TARGET_MAP)

sym: $(TARGET_SYM)

dump: $(TARGET_DUMP)

bin: $(TARGET_BIN)

srec: $(TARGET_SREC)

clean:
	$(RM) $(TARGET_AXF)
	$(RM) $(TARGET_MAP)
	$(RM) $(TARGET_SYM)
	$(RM) $(TARGET_DUMP)
	$(RM) $(TARGET_BIN)
	$(RM) $(TARGET_SREC)
	$(RM) $(LDS_GEN)
	$(RM) $(DPNS)
	$(RM) $(OBJS)

#---------------------------------------
# Dependence
#---------------------------------------
# Preprocess for ld script
$(LDS_GEN): $(LDS)
	$(GCC) -E -x c $(CFLAGS) $^ | grep -v '^#' > $@

# Asm file -> Depend file
.S.d:
	$(GCC) -MM -MD $<

# Asm file -> Object file
.S.o:
	$(GCC) -c $(ASFLAGS) $(CFLAGS) $<

# C file -> Depend file
.c.d:
	$(GCC) -MM -MD $<

# C file -> Object file
.c.o:
	$(GCC) -c $(CFLAGS) $<

# C file -> Asm file
.c.S:
	$(GCC) -S $(CFLAGS) $<

# Create AXF and map file
$(TARGET_AXF) $(TARGET_MAP): $(OBJS) $(LDS_GEN)
	$(GCC) $(CFLAGS) $(LDFLAGS) -Wl,-Map=$(TARGET_MAP) -o $(TARGET_AXF) $(OBJS)

# Create symbol file
$(TARGET_SYM): $(TARGET_AXF)
	$(NM) $^ > $@

# Create dump file
$(TARGET_DUMP): $(TARGET_AXF)
	$(OBJDUMP) -a -h -D -z $^ > $@

# Create binary
$(TARGET_BIN): $(TARGET_AXF)
	$(OBJCOPY) -O binary $^ $@

# Create S-record
$(TARGET_SREC): $(TARGET_AXF)
	$(OBJCOPY) -O srec $^ $@

# Include
-include $(DPNS)
