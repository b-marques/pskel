#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <mppaipc.h>
#include <iostream>
#include <fstream>
#include <string>

#define PSKEL_MPPA
#define MPPA_MASTER
// #define DEBUG
// #define BUG_TEST
// #define PRINT_OUT
// #define TIME_EXEC
// #define TIME_SEND
#include "../../include/PSkel.h"


using namespace std;
using namespace PSkel;


struct Arguments
{
	int externCircle;
	int internCircle;
	float power;
};


int CalcSize(int level){
	if (level == 1) {
    		return 3;
  	}
  	if (level >= 1) {
    		return CalcSize(level-1) + 2;
  	}
	return 0;
}



int main(int argc, char **argv){
	
	int width, height, tilingHeight, tilingWidth, iterations, innerIterations, nb_clusters, nb_threads; //stencil size
	if(argc != 9){
		printf ("Wrong number of parameters.\n");
		printf("Usage: WIDTH HEIGHT TILING_HEIGHT TILING_WIDTH ITERATIONS INNER_ITERATIONS NUMBER_CLUSTERS NUMBER_THREADS\n");
		exit(-1);
	}

	width = atoi(argv[1]);
	height = atoi(argv[2]);
	tilingHeight = atoi(argv[3]);
	tilingWidth = atoi(argv[4]);
	iterations = atoi(argv[5]);
	innerIterations = atoi(argv[6]);
	nb_clusters = atoi(argv[7]);
	nb_threads = atoi(argv[8]);

	//Mask configuration
	int level = 1;
	// int power = 2;
	int count = 0;
	int internCircle = pow(CalcSize(level), 2) - 1;
	int externCircle = pow(CalcSize(2*level), 2) - 1 - internCircle;
	int size = internCircle + externCircle;

  Mask2D<int> mask(size);

	for (int x = (level-2*level); x <= level; x++) {
		for (int y = (level-2*level); y <= level; y++) {
			if (x != 0 || y != 0) {
				mask.set(count, x, y);
				//cout<<count<<": ("<<x<<","<<y<<")"<<endl;
				count++;
			}
		}
  	}

  	for (int x = (2*level-4*level); x <= 2*level; x++) {
		for (int y = (2*level-4*level); y <= 2*level; y++) {
			if (x != 0 || y != 0) {
				if (!(x <= level && x >= -1*level && y <= level && y >= -1*level)) {
					mask.set(count, x, y);
					//cout<<count<<": ("<<x<<","<<y<<")"<<endl;
					count++;
				}
			}
		}
  	}
  int halo_value = mask.getRange()*innerIterations; 
  Array2D<int> inputGrid(width, height, halo_value); 
  Array2D<int> outputGrid(width, height, halo_value);
	
  //Arguments arg;
	count = 0;
	srand(1234);
	for(int h=0;h<height;h++) {
	    for(int w=0;w<width;w++) {
	    	// inputGrid(h,w) = h*width+w;
            inputGrid(h + halo_value, w + halo_value) = rand()%2;
	        #ifdef PRINT_OUT
//                printf("In position %d, %d we have %d\n", h, w, inputGrid(h,w));
            #endif
            // printf("inputGrid(%d,%d) = %d;\n", h, w, inputGrid(h,w));
            // outputGrid(h,w) = 0;
        }
    }

  // std::string grid;
  // for(int h = 0+ halo_value; h < height + halo_value; h++) {
  //  for(int w = 0 + halo_value; w < width + halo_value;  w++) {
  //   int element = inputGrid(h,w);
  //     char celement[10];
  //     sprintf(celement, " %d", element);
  //   grid+= celement;
  //  }
  //  grid += "\n";
  // }
  // std::cout << grid << std::endl;

	//Instantiate Stencil 2D
	Arguments arg;

	Stencil2D<Array2D<int>, Mask2D<int>, Arguments> stencil(inputGrid, outputGrid, mask, arg);

	//Schedule computation to slaves

  	stencil.scheduleMPPA("fur-async-slave", nb_clusters, nb_threads, width, height, tilingHeight, tilingWidth, iterations, innerIterations);

	// int outter_iterations = ceil(float(iterations)/innerIterations);
	// if(outter_iterations %2 == 1) {
	// 	grid = "";
	// 	for(int h=0+ halo_value; h < height + halo_value; h++) {
	// 	 for(int w=0+ halo_value; w < width + halo_value; w++) {
	// 	 	int element = outputGrid(h,w);
	// 			char celement[10];
	// 			sprintf(celement, " %d", element);
	// 	 	grid+= celement;
	// 	 }
	// 	 grid += "\n";
	// 	}
	// 	std::cout << "printing output" << std::endl;
	// 	std::cout << grid << std::endl;
	// } else { 
	// 	grid = "";
	// 	for(int h=0+ halo_value; h < height + halo_value; h++) {
	// 	 for(int w=0+ halo_value; w < width + halo_value; w++) {
	// 	 	int element = inputGrid(h,w);
	// 			char celement[10];
	// 			sprintf(celement, " %d", element);
	// 	 	grid+= celement;
	// 	 }
	// 	 grid += "\n";
	// 	}
	// 	std::cout << "printing input" << std::endl;
	// 	std::cout << grid << std::endl;
	// }
  // grid = "";
  // for(int h=0+ halo_value; h < height + halo_value; h++) {
  //  for(int w=0+ halo_value; w < width + halo_value; w++) {
  //   int element = outputGrid(h,w);
  //     char celement[10];
  //     sprintf(celement, " %d", element);
  //   grid+= celement;
  //  }
  //  grid += "\n";
  // }
  // std::cout << grid << std::endl;

  // grid = "";
  // for(int h=0+ halo_value; h < height + halo_value; h++) {
  //  for(int w=0+ halo_value; w < width + halo_value; w++) {
  //   int element = outputGrid(h,w);
  //     char celement[10];
  //     sprintf(celement, " %d", element);
  //   grid+= celement;
  //  }
  //  grid += "\n";
  // }
  // std::cout << grid << std::endl;

  // grid = "";
  // for(int h=0+ halo_value; h < height + halo_value; h++) {
  //  for(int w=0+ halo_value; w < width + halo_value; w++) {
  //   int element = inputGrid(h,w);
  //     char celement[10];
  //     sprintf(celement, " %d", element);
  //   grid+= celement;
  //  }
  //  grid += "\n";
  // }
  // std::cout << grid << std::endl;

	
  stencil.~Stencil2D();
	exit(0);
}
