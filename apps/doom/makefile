###############################################################################
# Makefile
###############################################################################
SRC_DIR=.
TARGET=doom
ARCH ?= riscv

MEM_SIZE     = 67108864
EXTRA_CFLAGS+= -DCONFIG_HEAP_MEMSIZE=33554432
EXTRA_CFLAGS+= -DNEWLIB_STACK_SIZE=1048576
EXTRA_CFLAGS+= -DCONFIG_UARTLITE
EXTRA_CFLAGS+= -DCONFIG_UARTLITE_CONSOLE
EXTRA_CFLAGS+= -DNORMALUNIX -std=gnu99 -O2

EXTRA_CFLAGS+= -DCONFIG_FRAMEBUFFER
CONFIG_TAR_FS_DIR=wad/

MAKE_DIR=../../make
include $(MAKE_DIR)/makefile.exe