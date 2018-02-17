#include <stdio.h>
#include <omp.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <algorithm>

#define PSKEL_MPPA
#define MPPA_SLAVE

#include "../../include/PSkel.h"

using namespace std;
using namespace PSkel;


#define WIND_X_BASE	15
#define WIND_Y_BASE	12
#define DISTURB		0.1f
#define CELL_LENGTH	0.1f
#define K           	0.0243f

int global_write_step;

struct Cloud{	
	Args2D<float> wind_x, wind_y;
	float deltaT;
	
	Cloud(){};
	
	Cloud(int linha, int coluna, int halo_value){		
		new (&wind_x) Args2D<float>(linha, coluna, halo_value);
		new (&wind_y) Args2D<float>(linha, coluna, halo_value);
	}
};

namespace PSkel{

	__parallel__ void stencilKernel(Array2D<float> &input,Array2D<float> &output,Mask2D<float> mask,Cloud cloud,size_t i, size_t j){
		int numNeighbor = 0;
		float sum = 0.0f;
		float inValue = input(i,j);

		float temperatura_vizinho = input(i-1,j);
		int factor = (temperatura_vizinho==0)?0:1;
		sum += factor*(inValue - temperatura_vizinho);
		numNeighbor += factor;

		temperatura_vizinho = input(i,j-1);
		factor = (temperatura_vizinho==0)?0:1;
		sum += factor*(inValue - temperatura_vizinho);
		numNeighbor += factor;
		
		temperatura_vizinho = input(i,j+1);
		factor = (temperatura_vizinho==0)?0:1;
		sum += factor*(inValue - temperatura_vizinho);
		numNeighbor += factor;

		temperatura_vizinho = input(i+1,j);
		factor = (temperatura_vizinho==0)?0:1;
		sum += factor*(inValue - temperatura_vizinho);
		numNeighbor += factor;

						
		float temperatura_conducao = -K*(sum / numNeighbor)*cloud.deltaT;
		
		float result = inValue + temperatura_conducao;
		
		float xwind = cloud.wind_x(i,j);
		float ywind = cloud.wind_y(i,j);
		int xfactor = (xwind>0)?3:1;
		int yfactor = (ywind>0)?2:0;

		float temperaturaNeighborX = input(xwind>0?i-1:i+1,j);
		float componenteVentoX = (xfactor-2)*xwind;

		float temperaturaNeighborY = input(i,ywind>0?j-1:j+1);
		float componenteVentoY = (yfactor-1)*ywind;
		
		float temp_wind = (-componenteVentoX * ((inValue - temperaturaNeighborX)/CELL_LENGTH)) -(componenteVentoY * ((inValue - temperaturaNeighborY)/CELL_LENGTH));
		
		output(i,j) = result + ((numNeighbor==4)?(temp_wind*cloud.deltaT):0.0f);
	}
}

int main(int argc, char **argv){
	int nb_tiles = atoi(argv[0]),
			tilling_width = atoi(argv[1]),
			tilling_height = atoi(argv[2]),
			cluster_id = atoi(argv[3]),
			nb_threads = atoi(argv[4]),
			inner_iterations = atoi(argv[5]),
			outter_iterations = atoi(argv[6]),
			itMod = atoi(argv[7]),
			nb_clusters = atoi(argv[8]),
			width = atoi(argv[9]),
			height = atoi(argv[10]),
			nb_computated_tiles = atoi(argv[11]);

	float deltaT = 0.01f;					//atof(argv[8]);

	Mask2D<float> mask(4);
	mask.set(0,0,1);
	mask.set(1,1,0);
	mask.set(2,0,-1);
	mask.set(3,-1,0);
		
  int halo_value = mask.getRange() * inner_iterations;

  Array2D<float> input(tilling_width, tilling_height, halo_value);
  Array2D<float> output(tilling_width, tilling_height, halo_value);
	
	Cloud cloud(tilling_width,tilling_height,halo_value);
	cloud.deltaT = deltaT;
	
	/* Inicialização dos ventos Latitudinal(Wind_X) e Longitudinal(Wind_Y) */
	for(int i = 0+halo_value; i < tilling_height+halo_value; i++ )
	{
		for(int j = 0+ halo_value; j < tilling_width + halo_value; j++ )
		{			
			cloud.wind_x(i,j) = (WIND_X_BASE - DISTURB) + (float)rand()/RAND_MAX * 2 * DISTURB;
			cloud.wind_y(i,j) = (WIND_Y_BASE - DISTURB) + (float)rand()/RAND_MAX * 2 * DISTURB;		
		}
	}

	Stencil2D<Array2D<float>, Mask2D<float>, Cloud> stencilCloud(input, output, mask, cloud);

	stencilCloud.runMPPA(cluster_id, nb_threads, nb_tiles, outter_iterations, inner_iterations, itMod, nb_clusters, width, height, nb_computated_tiles);
	
	return 0;
}
