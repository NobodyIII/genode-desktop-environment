LIBARCHIVE_PORT_DIR := $(call select_from_ports,libarchive)
LIBARCHIVE_DIR      := $(LIBARCHIVE_PORT_DIR)/src/lib/libarchive/libarchive

CC_DEF += -DHAVE_CONFIG_H

SRC_C = $(notdir $(wildcard $(LIBARCHIVE_DIR)/*.c))

vpath %.c $(LIBARCHIVE_DIR)

# find 'config.h'
INC_DIR += $(REP_DIR)/src/lib/libarchive

INC_DIR += $(LIBARCHIVE_DIR)

LIBS += libc zlib libbz2 libcrypto lz4 liblzma

SHARED_LIB = yes
