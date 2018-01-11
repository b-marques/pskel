#
# Copyright (C) 2015-2016 Kalray SA. All rights reserved.
#
ARCH ?= k1b
cluster_system ?= nodeos
K1_TOOLCHAIN_DIR ?= /usr/local/k1tools

arch := $(ARCH)
board := developer

system-name := bare
COMPILE_OPTI := -O3

# Cluster rules
cluster-bin := cluster_bin
cluster-system := $(cluster_system)
cluster_bin-srcs := slave.cpp ../../include/mppa_utils.cpp
cluster-cppflags := -g -DMPPA_TRACE_ENABLE ${COMPILE_OPTI} -mhypervisor -I . -Wall -fopenmp -std=c++14
cluster-lflags := -g -mhypervisor -lm -Wl,--defsym=USER_STACK_SIZE=0x2000 -Wl,--defsym=KSTACK_SIZE=0x1000 -lgomp
ifeq ($(cluster_system), bare)
cluster-lflags += -lvbsp -lutask -lmppa_remote -lmppa_async -lmppa_request_engine -lmppapower -lmppanoc -lmpparouting -Wl,--defsym=_LIBNOC_DISABLE_FIFO_FULL_CHECK=0
endif
ifeq ($(cluster_system), nodeos)
cluster-lflags += -pthread -lmppa_remote -lmppa_async -lmppa_request_engine -lmppapower -lmppanoc -lmpparouting -Wl,--defsym=_LIBNOC_DISABLE_FIFO_FULL_CHECK=0
endif

io-bin := io_bin
io_bin-srcs := master.cpp
io_bin-cppflags := -g ${COMPILE_OPTI} -DMPPA_TRACE_ENABLE -Wall -mhypervisor -std=c++14
io_bin-lflags := -lvbsp -lmppa_remote -lmppa_async -lmppa_request_engine -lpcie_queue -lutask  -lmppapower -lmppanoc -lmpparouting -mhypervisor -Wl,--defsym=_LIBNOC_DISABLE_FIFO_FULL_CHECK=0 -Wl,--defsym=_start_async_copy=0

mppa-bin := multibin_bin
multibin_bin-objs = io_bin cluster_bin

host-bin := host_bin
host_bin-srcs := host_main.c
host-cppflags := -Wall ${COMPILE_OPTI} -g
host-lflags := -lpthread -lm -lrt -lmppa_remote -lpcie
host-bin    := host_bin

clean-hooks := clean1

include $(K1_TOOLCHAIN_DIR)/share/make/Makefile.kalray

run_sim: all
	$(K1_TOOLCHAIN_DIR)/bin/k1-mppa --march=bostan -- ${O}/bin/multibin_bin.mpk

run_pcie_sim: all
	$(K1_TOOLCHAIN_DIR)/bin/k1-pciesim-runner ./${O}/bin/host_bin ./${O}/bin/multibin_bin.mpk io_bin


ifeq (${board}, emb01)
# When running on emb01, the following variable must be defined.
# k1-remote-runner will also need a password. We STRONGLY recommend
# to do not use --passwd option for that.
# Instead, please consider providing the proper user ssh public key
# from developer to the EMB01 (you may use ssh-copyid standard tool).
# It will allow connecting without providing a password.
EMB01_ADDRESS := $(shell echo $$EMB01_ADDRESS)
EMB01_USER := $(shell echo $$EMB01_USER)

run_jtag: all
	@[ "" != "${EMB01_ADDRESS}" ]  || ( echo "EMB01_ADDRESS not defined"  ; false)
	@[ "" != "${EMB01_USER}" ]     || ( echo "EMB01_USER not defined"     ; false)
	cd $(BIN_DIR);k1-remote-runner --remote ${EMB01_ADDRESS} --user ${EMB01_USER} --host_exec host_bin --multibinary multibin_bin.mpk -- ./host_bin multibin_bin.mpk io_bin

run_hw: all
	@[ "" != "${EMB01_ADDRESS}" ]  || ( echo "EMB01_ADDRESS not defined"  ; false)
	@[ "" != "${EMB01_USER}" ]     || ( echo "EMB01_USER not defined"     ; false)
	cd $(BIN_DIR) 
else

run_jtag: all
	$(K1_TOOLCHAIN_DIR)/bin/k1-jtag-runner --multibinary=./${O}/bin/multibin_bin.mpk --exec-multibin=IODDR0:io_bin

run_hw: all
	./${O}/bin/host_bin ./${O}/bin/multibin_bin.mpk io_bin

endif

clean1:
	rm -fr ${O} *~ MPPA.* bostan.*.trace* tracefile.* traces/ profile/ tracedtv/ callgrind/