#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <mppaipc.h>

#define PSKEL_MPPA
#define MPPA_MASTER
//#define DEBUG
//#define BUG_TEST
// #define PRINT_OUT
// #define TIME_EXEC
// #define TIME_SEND
#include "../../include/PSkel.h"

using namespace std;
using namespace PSkel;


struct Arguments
{
	int dummy;
};

int main(int argc, char **argv){
	int width, 
			height,
			tilingHeight,
			tilingWidth,
			iterations,
			innerIterations,
			// pid,
			nb_clusters,
			nb_threads; //stencil size
	if(argc != 9){
		printf ("Wrong number of parameters.\n");
		printf("Usage: WIDTH HEIGHT TILING_HEIGHT TILING_WIDTH ITERATIONS INNER_ITERATIONS NUMBER_CLUSTERS NUMBER_THREADS\n");
		exit(-1);
	}

	//Stencil configuration
	width = atoi(argv[1]);
	height = atoi(argv[2]);
  tilingHeight = atoi(argv[3]);
  tilingWidth = atoi(argv[4]);
	iterations = atoi(argv[5]);
	innerIterations = atoi(argv[6]);
	nb_clusters = atoi(argv[7]);
	nb_threads = atoi(argv[8]);
	//printf("nb_threads: %d\n", nb_threads);
	//Mask configuration

  Mask2D<int> mask(8);
	mask.set(0,-1,-1);	mask.set(1,-1,0);	mask.set(2,-1,1);
	mask.set(3,0,-1);											mask.set(4,0,1);
	mask.set(5,1,-1);		mask.set(6,1,0);	mask.set(7,1,1);
	
	int halo_value = mask.getRange()*innerIterations;
	Array2D<int> inputGrid(width, height, halo_value);
	Array2D<int> outputGrid(width, height, halo_value);

	srand(1234);
	for(int h = 0; h < height + halo_value*2; h++) {
		for(int w = 0; w < width + halo_value*2; w++) {
			// inputGrid(h,w) = 0;
			// inputGrid(h + halo_value, w + halo_value) = rand()%2;
			// inputGrid(h + halo_value,w + halo_value) = h*width+w;
	    // printf("inputGrid(%d,%d) = %d;\n", h, w, inputGrid(h  + halo_value, w + halo_value));
      // outputGrid(h + halo_value, w + halo_value) = rand()%2;
		}
	}

	inputGrid(0 + halo_value,0 + halo_value) = 1;
	inputGrid(0 + halo_value,1 + halo_value) = 1;
	inputGrid(1 + halo_value,0 + halo_value) = 1;
	inputGrid(1 + halo_value,1 + halo_value) = 1;	

	inputGrid(2 + halo_value,2 + halo_value) = 1;
	inputGrid(2 + halo_value,3 + halo_value) = 1;
	inputGrid(3 + halo_value,2 + halo_value) = 1;
	inputGrid(3 + halo_value,3 + halo_value) = 1;	

	// for(int h=0;h<height;h++) {
	// 	for(int w=0;w<width;w++) {
	// 	  printf("inputGrid(%d,%d) = %d;\n", h, w, inputGrid(h,w));
	// 	}
	// }

	// for(int h=0;h<height;h++) {
	// 	for(int w=0;w<width;w++) {
      // printf("outputGrid(%d,%d) = %d;\n", h, w, outputGrid(h,w));
	// 	}
	// }

	std::string grid;
  for(int h = 0; h < height + halo_value*2; h++) {
    for(int w = 0 ; w < width + halo_value*2;  w++) {
    	int element = inputGrid(h,w);
			char celement[10];
			sprintf(celement, " %d", element);
    	grid+= celement;
    }
    grid += "\n";
	}
	std::cout << grid << std::endl;

	//Instantiate Stencil 2D
	Stencil2D<Array2D<int>, Mask2D<int>, Arguments> stencil(inputGrid, outputGrid, mask);
	//struct timeval start = mppa_master_get_time();

	//Schedule computation to slaves
	stencil.scheduleMPPA("cluster_bin", nb_clusters, nb_threads, width, height, tilingHeight, tilingWidth, iterations, innerIterations);

	//struct timeval end=mppa_master_get_time();
	//cout<<"Master Time: " << mppa_master_diff_time(start,end) << endl;


	// for(int h=0;h<height;h++) {
	// 	for(int w=0;w<width;w++) {
 //      printf("outputGrid(%d,%d) = %d;\n", h, w, outputGrid(h,w));
	// 	}
	// }

	// 	for(int h=0;h<height;h++) {
	// 	for(int w=0;w<width;w++) {
 //      printf("inputGrid(%d,%d) = %d;\n", h, w, inputGrid(h,w));
	// 	}
	// }

		grid = "";

	  for(int h=0; h < height + halo_value*2; h++) {
      for(int w=0; w < width + halo_value*2; w++) {
      	int element = outputGrid(h,w);
				char celement[10];
				sprintf(celement, " %d", element);
      	grid+= celement;
      }
      grid += "\n";
  }
  std::cout << grid << std::endl;



	exit(0);
}
