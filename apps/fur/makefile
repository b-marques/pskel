#
# Copyright (C) 2015-2016 Kalray SA. All rights reserved.
#

K1_TOOLCHAIN_DIR=/usr/local/k1tools
system-name := bare

# Directories.
BINDIR  = $(CURDIR)/bin
INCDIR  = $(CURDIR)/../../include
SRCDIR  = $(CURDIR)

# Toolchain Configuration
cppflags := -std=c++14
cppflags += -Wall
cppflags += -O3
cppflags += -I $(INCDIR)
cppflags += -D_KALRAY_MPPA256
lflags := -Wl,--defsym=_LIBNOC_DISABLE_FIFO_FULL_CHECK=0

#=============================================================================
# Game of Life - Async API
#=============================================================================

io-bin += fur-async-master
fur-async-master-srcs := $(SRCDIR)/master.cpp
 
# Toolchain Configuration
fur-async-master-system := bare
fur-async-master-lflags := -mhypervisor -lutask -lmppa_async -lmppa_request_engine
fur-async-master-lflags += -lmppapower -lmppanoc -lmpparouting
fur-async-master-lflags += -lpcie_queue


cluster-bin += fur-async-slave
fur-async-slave-srcs := $(SRCDIR)/slave.cpp       \
                        $(INCDIR)/mppa_utils.cpp 

# Toolchain Configuration
fur-async-slave-system   := bare
fur-async-slave-cppflags += -fopenmp
fur-async-slave-lflags   := -mhypervisor -lutask -lmppa_async -lmppa_request_engine 
fur-async-slave-lflags   += -lmppapower -lmppanoc -lmpparouting -lgomp
fur-async-slave-lflags   += -Wl,--defsym=USER_STACK_SIZE=0x2000
fur-async-slave-lflags   += -Wl,--defsym=KSTACK_SIZE=0x1000


fur-async-objs := fur-async-master fur-async-slave
fur-async-name := fur-async.img

mppa-bin := fur-async

include $(K1_TOOLCHAIN_DIR)/share/make/Makefile.kalray