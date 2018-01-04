/**
 * Copyright (C) 2016-2017 Kalray SA.
 *
 * All rights reserved.
 *
 * An application is implemented on a single cluster
 *     and parallelized from one to 16 cores 
 *     without communications between clusters
 *
 * Exercise 1.1 / Training MPPA Distributed C/C++ Programming
 */

#include <stdio.h>
#include <stdlib.h>
#include <pcie.h>

int main(int argc, const char **argv)
{

/*******************************************************************************
 *                  	HOST INITIALIZATION CODE                                
 *                     !!!! DO NOT MODIFY !!!!
 *******************************************************************************
 * This code is in charge of:
 *   - Initializing the PCIe queue
 *   - Initializing Syscall remoting library
 *   - Spawning the MPPA binary to MPPA
*******************************************************************************/

	mppadesc_t fd;
	int mppa_ret;

	if(!(fd = pcie_open_device(0))) 
		exit(-1);
	// printf("[HOST] Starts MPPA\n");
	if(pcie_load_io_exec_args_mb(fd, "./output/bin/multibin_bin.mpk", "io_bin", argv, argc, PCIE_LOAD_FULL)){
		printf ("Boot of MPPA failed\n");
		exit(1);
	}
	// printf("# [HOST] pcie queue init\n");
	pcie_queue_init(fd);
	/* pcie_queue init needs to be called to enable pcie communication via queues */ 
	//printf("# [HOST] init queue ok\n");	
	pcie_register_console(fd, stdin, stdout);
	//printf("# [HOST] pcie_register_console ok\n");
	int status;
	int local_status = 0;

/*******************************************************************************
* 			USER CODE IF NEEDED
*******************************************************************************/
	
/******************************************************************************/

/*******************************************************************************
 *			HOST END CODE                                
 *			!!!! DO NOT MODIFY !!!!
 *******************************************************************************
 * This code is in charge of:
 *   - Destroying the PCIe queue
 *   - Finishing the host run
*******************************************************************************/

	//printf("# [HOST] waits\n");	
	pcie_queue_barrier(fd, local_status, &status);
	pcie_queue_exit(fd, 0xDEAD, &mppa_ret);
	//printf("# [HOST] MPPA exited with status %d\n", status);
	pcie_close(fd);
	//printf("# [HOST] Goodbye\n");
 	return 0;

/***********************END HOST END CODE**************************************/

}