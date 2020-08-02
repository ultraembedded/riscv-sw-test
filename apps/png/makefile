###############################################################################
# Makefile
###############################################################################
SRC_DIR=.
TARGET=png
ARCH ?= riscv

MEM_SIZE     = 8388608
EXTRA_CFLAGS+= -DNEWLIB_STACK_SIZE=1048576
EXTRA_CFLAGS+= -DCONFIG_UARTLITE
EXTRA_CFLAGS+= -DCONFIG_UARTLITE_CONSOLE

EXTRA_CFLAGS+= -DCONFIG_FRAMEBUFFER
CONFIG_TAR_FS_DIR=images/

MAKE_DIR=../../make
include $(MAKE_DIR)/makefile.exe