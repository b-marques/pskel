cmd_output/bin/io_bin := /usr/local/k1tools/bin/k1-g++ -o output/bin/io_bin    output/build/io_bin_build/master.cpp.o   -mcluster=ioddr -L/home/lig-ext/marquesb/project/pskel/apps/fur/output/lib/io/    -march=k1b -mboard=developer -mos=bare -lvbsp -lmppa_remote -lmppa_async -lmppa_request_engine -lpcie_queue -lutask  -lmppapower -lmppanoc -lmpparouting -mhypervisor -Wl,--defsym=_LIBNOC_DISABLE_FIFO_FULL_CHECK=0 -Wl,--defsym=_start_async_copy=0  
