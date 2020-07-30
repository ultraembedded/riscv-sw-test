###############################################################################
# Makefile
###############################################################################
SRC_DIR=.
TARGET=test

MEM_SIZE     = 8388608
EXTRA_CFLAGS+= -DNEWLIB_STACK_SIZE=1048576
EXTRA_CFLAGS+= -DCONFIG_UARTLITE
EXTRA_CFLAGS+= -DCONFIG_UARTLITE_CONSOLE

MAKE_DIR=../../make
include $(MAKE_DIR)/makefile.exe