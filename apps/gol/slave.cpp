#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PSKEL_MPPA
#define MPPA_SLAVE

#include "../../include/mppa_utils.h"
#include "../../include/PSkel.h"


// #define DEBUG
//#define BUG_TEST
// #define PRINT_OUT
// #define TIME_EXEC
// #define TIME_SEND
// #define BARRIER_SYNC_MASTER "/mppa/sync/128:1"
// #define BARRIER_SYNC_SLAVE "/mppa/sync/[0..15]:2"
//#include "common.h"

using namespace std;
using namespace PSkel;


struct Arguments
{
  int dummy;
};


namespace PSkel{
__parallel__ void stencilKernel(Array2D<int> &input, Array2D<int> &output, Mask2D<int> mask, Arguments args, size_t i, size_t j){
    int neighbors =  input(i-1,j-1) + input(i-1,j) + input(i-1,j+1)  +
                    input(i+1,j-1) + input(i+1,j) + input(i+1,j+1)  +
                    input(i,j-1)   + input(i,j+1) ;


    bool central = input(i,j);
    // int neighbors = mask.get(0,input,i,j) + mask.get(1,input,i,j) + mask.get(2,input,i,j) +
    //                mask.get(3,input,i,j) + mask.get(4,input,i,j) + mask.get(5,input,i,j) +
    //          mask.get(6,input,i,j) + mask.get(7,input,i,j);
    
    

  //   int neighbors = 0;
  //   int height=input.getHeight();
  //   int width=input.getWidth();
  //
  //   if ( (j == 0) && (i == 0) ) { //  Corner 1
  //       neighbors = input(i+1,j) + input(i,j+1) + input (i+1,j+1);
  //   } //  Corner 2
  //   else if ((j == 0) && (i == width-1)) {
  //       neighbors = input(i-1,j) + input(i,j+1) + input(i-1,j+1);
  //   } //  Corner 3
  //   else if ((j == height-1) && (i == width-1)) {
  //       neighbors = input(i-1,j) + input(i,j-1) + input(i-1,j-1);
  //   } //  Corner 4
  //   else if ((j == height-1) && (i == 0)) {
  //       neighbors = input(i,j-1) + input(i+1,j) + input(i+1,j-1);
  //   } //  Edge 1
  //   else if (j == 0) {
  //       neighbors = input(i-1,j) + input(i+1,j) + input(i-1,j+1) + input(i,j+1) + input(i+1,j+1);
  //   } //  Edge 2
  //   else if (i == width-1) {
  //       neighbors = input(i,j-1) +  input(i-1,j-1) + input(i-1,j) +  input(i-1,j+1) + input(i,j+1);
  //   } //Edge 3
  //   else if (j == height-1) {
  //       neighbors = input(i-1,j-1) + input(i,j-1) + input(i+1,j-1) + input(i-1,j) + input(i+1,j);
  //   } //Edge 4
  //   else if (i == 0) {
  //       neighbors = input(i,j-1) + input(i+1,j-1) + input(i+1,j) + input(i,j+1) + input(i+1,j+1);
  //   } //Inside the grid
  //   else {
  //       neighbors =  input(i-1,j-1) + input(i-1,j) + input(i-1,j+1)  +
  //                    input(i+1,j-1) + input(i+1,j) + input(i+1,j+1)  +
  //                    input(i,j-1)   + input(i,j+1) ;
  //   }
  //
  output(i,j) = (neighbors == 3 || (neighbors == 2 && central))?1:0;
  // output(i,j) = input(i,j);
  }
}

int main(int argc,char **argv) {

   /**************Mask for test porpuses****************/
  Arguments arg;
  arg.dummy = 1;
  Mask2D<int> mask(8);
  mask.set(0,-1,-1);  mask.set(1,-1,0); mask.set(2,-1,1);
  mask.set(3,0,-1);                     mask.set(4,0,1);
  mask.set(5,1,-1); mask.set(6,1,0);    mask.set(7,1,1);
  /*************************************************/

  // width = atoi (argv[1]);
  // height = atoi (argv[2]);
  // iterations = atoi (argv[3]);
  // mode = atoi(argv[4]);
  // GPUBlockSize = atoi(argv[5]);
  // numCPUThreads = atoi(argv[6]);
  // tileHeight = atoi(argv[7]);
  // tileIterations = atoi(argv[8]);

  int nb_tiles = atoi(argv[0]);
  int tilling_width = atoi(argv[1]);
  int tilling_height = atoi(argv[2]);
  int cluster_id = atoi(argv[3]);
  int nb_threads = atoi(argv[4]);
  int inner_iterations = atoi(argv[5]);
  int outter_iterations = atoi(argv[6]);
  int itMod = atoi(argv[7]);
  int nb_clusters = atoi(argv[8]);
  int width = atoi(argv[9]);
  int height = atoi(argv[10]);
  int nb_computated_tiles = atoi(argv[11]);

  int halo_value = mask.getRange() * inner_iterations;

  Array2D<int> partInput(tilling_width, tilling_height, halo_value);
  Array2D<int> output(tilling_width, tilling_height, halo_value);
  Stencil2D<Array2D<int>, Mask2D<int>, Arguments> stencil(partInput, output, mask, arg);
  // if(iterations == 0)  {

  stencil.runMPPA(cluster_id, nb_threads, nb_tiles, outter_iterations, inner_iterations, itMod, nb_clusters, width, height, nb_computated_tiles);
  
  // } else {
  //      stencil.runIterativeMPPA(cluster_id, nb_threads, nb_tiles, iterations);
  //}
  return 0;
}
