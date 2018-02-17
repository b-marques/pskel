#include <stdio.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <algorithm>

#define PSKEL_MPPA
#define MPPA_MASTER

#include "../../include/PSkel.h"

using namespace std;
using namespace PSkel;


#define WIND_X_BASE 15
#define WIND_Y_BASE 12
#define DISTURB   0.1f
#define CELL_LENGTH 0.1f
#define K             0.0243f
#define DELTAPO         0.5f
#define TAM_VETOR_FILENAME  200

int numThreads;
int count_write_step;
int global_write_step;
int global_count_write_step;
char dirname[TAM_VETOR_FILENAME];
char *typeSim;

struct Cloud{ 
  Args2D<float> wind_x, wind_y;
  float deltaT;
  
  Cloud(){};
  
  Cloud(int linha, int coluna, int halo_value){   
    new (&wind_x) Args2D<float>(linha, coluna, halo_value);
    new (&wind_y) Args2D<float>(linha, coluna, halo_value);
  }
};

/* Convert Celsius to Kelvin */
float Convert_Celsius_To_Kelvin(float number_celsius)
{
  float number_kelvin;
  number_kelvin = number_celsius + 273.15f;
  return number_kelvin;
}

/* Convert Pressure(hPa) to Pressure(mmHg) */
float Convert_hPa_To_mmHg(float number_hpa)
{
  float number_mmHg;
  number_mmHg = number_hpa * 0.750062f;

  return number_mmHg;
}

/* Convert Pressure Millibars to mmHg */
float Convert_milibars_To_mmHg(float number_milibars)
{
  float number_mmHg;
  number_mmHg = number_milibars * 0.750062f;

  return number_mmHg;
}

/* Calculate RPV */
float CalculateRPV(float temperature_Kelvin, float pressure_mmHg)
{
  float realPressureVapor; //e
  float PsychrometricConstant = 6.7f * powf(10,-4); //A
  float PsychrometricDepression = 1.2f; //(t - tu) in ºC
  float esu = pow(10, ((-2937.4f / temperature_Kelvin) - 4.9283f * log10(temperature_Kelvin) + 23.5470f)); //10 ^ (-2937,4 / t - 4,9283 log t + 23,5470)
  realPressureVapor = Convert_milibars_To_mmHg(esu) - (PsychrometricConstant * pressure_mmHg * PsychrometricDepression);

  return realPressureVapor;
}

/* Calculate Dew Point */
float CalculateDewPoint(float temperature_Kelvin, float pressure_mmHg)
{
  float dewPoint; //TD
  float realPressureVapor = CalculateRPV(temperature_Kelvin, pressure_mmHg); //e
  dewPoint = (186.4905f - 237.3f * log10(realPressureVapor)) / (log10(realPressureVapor) -8.2859f);

  return dewPoint;
}

/*Calculate temperature grid standard deviation */
void StandardDeviation(Array2D<float> grid, int linha, int coluna, int iteracao, char *dirname, int numThreads, char *typeSim, float average)
{
  FILE *file;
  char filename[TAM_VETOR_FILENAME];
  double desviopadrao = 0;
  double variancia = 0; 

  if(numThreads > 1)    
    sprintf(filename,"%s//%s-desviopadrao-temperature-simulation-%d-thread.txt",dirname, typeSim,numThreads);
  else    
    sprintf(filename,"%s//%s-desviopadrao-temperature-simulation.txt",dirname, typeSim);
  file = fopen(filename, "a");  
  
  for(int i = 0; i < linha; i++)
  {
    for(int j = 0; j < coluna; j++)
    {     
      variancia+=pow((grid(i,j)-average),2);
    }   
   }  
   desviopadrao = sqrt(variancia/(linha*coluna-1));
   fprintf(file,"%d\t%lf\n",iteracao, desviopadrao);
   fclose(file);
}

/*Calculate temperature grid average */
void CalculateAverage(Array2D<float> grid, int linha, int coluna, int iteracao, char *dirname, int numThreads, char *typeSim)
{
  FILE *file;
  char filename[TAM_VETOR_FILENAME];
  double sum = 0;
  double average = 0; 

  if(numThreads > 1)    
    sprintf(filename,"%s//%s-average-temperature-simulation-%d-thread.txt",dirname, typeSim,numThreads);    
  else    
    sprintf(filename,"%s//%s-average-temperature-simulation.txt",dirname, typeSim);
  file = fopen(filename, "a");  
  

  for(int i = 0; i < linha; i++)
  {   
    for(int j = 0; j < coluna; j++)
    {     
      sum+=grid(i,j);
    }   
   }  
   average = sum/(linha*coluna);
   //fprintf(file,"%d\t%lf\n",iteracao, average);
   fprintf(file,"%lf\n", average);
   printf("Media:%d\t%lf\n",iteracao, average);
   fclose(file);
   
  // StandardDeviation(grid, linha, coluna, iteracao, dirname, numThreads, typeSim, average);
   
}

/* Calculate statistics of temperature grid */
void CalculateStatistics(Array2D<float> grid, int linha, int coluna, int iteracao, char *dirname, int numThreads, char *typeSim)
{
  CalculateAverage(grid, linha, coluna, iteracao, dirname, numThreads, typeSim);  
}


/* Write grid temperature in text file */
void WriteGridTemp(Array2D<float> grid, int linha, int coluna, int iteracao, int numThreads, char *dirname, char *typeSim)
{
  FILE *file;
  char filename[TAM_VETOR_FILENAME];
  
  if(numThreads > 1)
    sprintf(filename,"%s//%s-temp_%d-thread_iteration#_%d.txt",dirname, typeSim, numThreads, iteracao);
  else
    sprintf(filename,"%s//%s-temp_iteration#_%d.txt",dirname, typeSim, iteracao);
    
  file = fopen(filename, "w");    

  fprintf(file, "Iteração: %d\n", iteracao);
  for(int i = 0; i < linha; i++)
  {
    for(int j = 0; j < coluna; j++)
    {
      fprintf(file, "%.4f  ", grid(i,j));
    }
    fprintf(file, "\n");
   }    
  fclose(file);
}

/* Write time simulation */
void WriteTimeSimulation(float time, int numThreads, char *dirname, char *typeSim)
{
  FILE *file;
  char filename[TAM_VETOR_FILENAME];
  
  if(numThreads > 1)
    sprintf(filename,"%s//%stime-sim_%d-thread.txt",dirname, typeSim, numThreads);
  else
    sprintf(filename,"%s//%stime-sim.txt",dirname, typeSim);

  file = fopen(filename,"r");
  if (file==NULL)
  {
    file = fopen(filename, "w");
    //fprintf(file,"Time %s-simulation", typeSim);
    fprintf(file,"\nUpdate Time: %f segundos", time);
  }
  else
  {
    file = fopen(filename, "a");
    fprintf(file,"\nUpdate Time: %f segundos", time);
  }
  
  fclose(file);
}

/* Write Simulation info all parameters */
void WriteSimulationInfo(int numero_iteracoes, int linha, int coluna, int raio_nuvem, float temperaturaAtmosferica, float alturaNuvem, float pressaoAtmosferica, float deltaT, float pontoOrvalho, int menu_option, int write_step, int numThreads, float GPUTime, char *dirname, char *typeSim)
{ 
  FILE *file;
  char filename[TAM_VETOR_FILENAME];
  sprintf(filename,"%s//%s-simulationinfo.txt",dirname, typeSim);
  
  file = fopen(filename,"r");
  if (file==NULL)
  {   
    file = fopen(filename, "w");
    fprintf(file,"***Experimento %s***", typeSim);
    fprintf(file,"\nData_%s",__DATE__);
    if(numThreads > 1){
    fprintf(file,"\nNúmero de Threads:%d", numThreads);
    fprintf(file,"\nProporção GPU:%.1f", GPUTime*100);
    fprintf(file,"\nProporção CPU:%.1f", (1.0-GPUTime)*100);
    }
    fprintf(file,"\nTemperatura Atmosférica:%.1f", temperaturaAtmosferica);
    fprintf(file,"\nAltura da Nuvem:%.1f", alturaNuvem);
    fprintf(file,"\nPonto de Orvalho:%f", pontoOrvalho);
    fprintf(file,"\nPressao:%.1f", pressaoAtmosferica);
    fprintf(file,"\nCondutividade térmica:%f", K);
    fprintf(file,"\nDeltaT:%f", deltaT);    
    fprintf(file,"\nNúmero de Iterações:%d", numero_iteracoes);
    fprintf(file,"\nTamanho da Grid:%dX%d", linha, coluna);
    fprintf(file,"\nRaio da nuvem:%d", raio_nuvem);
    // fprintf(file,"\nNúmero de Processadores do Computador:%d", omp_get_num_procs());
    fprintf(file,"\nDelta Ponto de Orvalho:%f", DELTAPO);
    fprintf(file,"\nLimite Inferior Ponto de Orvalho:%lf", (pontoOrvalho - DELTAPO));
    fprintf(file,"\nLimite Superior Ponto de Orvalho:%lf", (pontoOrvalho + DELTAPO));
    fprintf(file,"\nMenu Option:%d", menu_option);
    fprintf(file,"\nWrite Step:%d", write_step);
    
    fclose(file);
  }
  else
  {
    char filename_old[TAM_VETOR_FILENAME];
    string line;
    int posicao;
    char buffer [33];

    sprintf(filename_old,"%s//file_temp.txt",dirname);
    ofstream outFile(filename_old, ios::out);
    ifstream fileread(filename);

        while(!fileread.eof())
    {
      getline(fileread, line);
      posicao = line.find("Threads:");
      if (posicao!= string::npos)
      {
        string line_temp = line.substr(posicao+1, line.size());
        sprintf (buffer, "%d", numThreads);
        posicao = line_temp.find(buffer);
        if (posicao!= string::npos)
        {
          outFile << line << '\n';
        }
        else
        {
          sprintf (buffer, ",%d", numThreads);
          line.append(buffer);
          outFile << line << '\n';
        }
      }
      else
        outFile << line << '\n';
    }
    remove(filename);
    rename(filename_old, filename);
    outFile.close();
    fileread.close();
  } 
}

/* Write grid wind in text file */
void WriteGridWind(Cloud cloud, int linha, int coluna, char *dirname_windx, char *dirname_windy)
{
  FILE *file_windx, *file_windy;
  char filename_windx[TAM_VETOR_FILENAME];
  char filename_windy[TAM_VETOR_FILENAME];

  sprintf(filename_windx,"%s//windX.txt",dirname_windx);
  sprintf(filename_windy,"%s//windY.txt",dirname_windy);
  file_windx = fopen(filename_windx,"r");
  file_windy = fopen(filename_windy, "r");

        if (file_windx == NULL && file_windy == NULL)
  {
    file_windx = fopen(filename_windx, "w");
          file_windy = fopen(filename_windy, "w");
    for(int i = 0; i < linha; i++)
    {
      for(int j = 0; j < coluna; j++)
      {
      fprintf(file_windx, "%.4f  ", cloud.wind_x(i,j));
      fprintf(file_windy, "%.4f  ", cloud.wind_y(i,j));
      }
    fprintf(file_windx, "\n");
    fprintf(file_windy, "\n");
    }
  }
  fclose(file_windx);
        fclose(file_windy);
}


int main(int argc, char **argv){

  if(argc != 9){
    printf ("Wrong number of parameters.\n");
    printf("Usage: WIDTH HEIGHT TILLING_HEIGHT TILLING_WIDTH ITERATIONS INNER_ITERATIONS NUMBER_CLUSTERS NUMBER_THREADS\n");
    exit(-1);
  }
  int raio_nuvem, write_step;
  float temperaturaAtmosferica, pressaoAtmosferica, pontoOrvalho, limInfPO, limSupPO, deltaT;
  //float alturaNuvem;

  int width, 
      height,
      tilling_height,
      tilling_width,
      iterations,
      inner_iterations,
      // pid,
      nb_clusters,
      nb_threads; //stencil size

  //Stencil configuration
  width = atoi(argv[1]);
  height = atoi(argv[2]);
  tilling_height = atoi(argv[3]);
  tilling_width = atoi(argv[4]);
  iterations = atoi(argv[5]);
  inner_iterations = atoi(argv[6]);
  nb_clusters = atoi(argv[7]);
  nb_threads = atoi(argv[8]);
  //printf("nb_threads: %d\n", nb_threads);
  //Mask configuration


  raio_nuvem = 20;        //atoi(argv[4]);
  temperaturaAtmosferica = -3.0f;   //atof(argv[5]);
  //alturaNuvem = 5.0;        //atof(argv[6]);
  pressaoAtmosferica =  700.0f;   //atof(argv[7]);
  deltaT = 0.01f;         //atof(argv[8]);


  write_step = 10;        //atoi(argv[13]);
  
  global_write_step = write_step;
  pontoOrvalho = CalculateDewPoint(Convert_Celsius_To_Kelvin(temperaturaAtmosferica), Convert_hPa_To_mmHg(pressaoAtmosferica));
  limInfPO = pontoOrvalho - DELTAPO;
  limSupPO = pontoOrvalho + DELTAPO;
  //char maindir[30];
  //char dirname[TAM_VETOR_FILENAME];
  //char dirMatrix_temp[TAM_VETOR_FILENAME];
  //char dirMatrix_stat[TAM_VETOR_FILENAME];
  //char dirMatrix_windX[TAM_VETOR_FILENAME];
  //char dirMatrix_windY[TAM_VETOR_FILENAME];
  //float start_time = 0;
  //float end_time = 0;

  Mask2D<float> mask(4);
  
  mask.set(0,0,1);
  mask.set(1,1,0);
  mask.set(2,0,-1);
  mask.set(3,-1,0);
    
  int halo_value = mask.getRange() * inner_iterations;

  Array2D<float> inputGrid(width, height, halo_value);
  Array2D<float> outputGrid(width, height, halo_value);
  
  Cloud cloud(height,width, halo_value);
  cloud.deltaT = deltaT;

  srand(1234);
  for(int h = 0 + halo_value; h < height + halo_value; h++) {
    for(int w = 0 + halo_value; w < width + halo_value; w++) {
      inputGrid(h,w) = temperaturaAtmosferica;
    }
  }

  /* Inicialização de uma nuvem no centro da matriz de entrada */
  int y, x0 = height/2, y0 = width/2;
  srand(1);
  for(int i = x0 - raio_nuvem; i < x0 + raio_nuvem; i++)
  {
     // Equação da circunferencia: (x0 - x)² + (y0 - y)² = r²
    y = (int)((floor(sqrt(pow((float)raio_nuvem, 2.0) - pow(((float)x0 - (float)i), 2)) - y0) * -1));

    for(int j = y0 + (y0 - y); j >= y; j--)
    {
      inputGrid(i+halo_value,j+halo_value) = limInfPO + (float)rand()/RAND_MAX * (limSupPO - limInfPO);
      //outputGrid(i,j) = limInfPO + (float)rand()/RAND_MAX * (limSupPO - limInfPO);
    }
  }
  
  Stencil2D<Array2D<float>, Mask2D<float>, Cloud> stencilCloud(inputGrid, outputGrid, mask, cloud);

  std::string grid;
  for(int h = 0+halo_value; h < height+halo_value; h++) {
   for(int w = 0+halo_value; w < width+halo_value;  w++) {
    float element = inputGrid(h,w);
      char celement[10];
      sprintf(celement, " %.1f", element);
    grid+= celement;
   }
   grid += "\n";
  }
  std::cout << grid << std::endl;

  stencilCloud.scheduleMPPA("cluster_bin", nb_clusters, nb_threads, width, height, tilling_height, tilling_width, iterations, inner_iterations);
  
  int outter_iterations = ceil(float(iterations)/inner_iterations);
  if(outter_iterations %2 == 1) {
    grid = "";
    for(int h=0+ halo_value; h < height + halo_value; h++) {
     for(int w=0+ halo_value; w < width + halo_value; w++) {
      float element = outputGrid(h,w);
        char celement[10];
        sprintf(celement, " %.1f", element);
      grid+= celement;
     }
     grid += "\n";
    }
    std::cout << "printing output" << std::endl;
    std::cout << grid << std::endl;
  } else { 
    grid = "";
    for(int h=0+ halo_value; h < height + halo_value; h++) {
     for(int w=0+ halo_value; w < width + halo_value; w++) {
      float element = inputGrid(h,w);
        char celement[10];
        sprintf(celement, " %.2f", element);
      grid+= celement;
     }
     grid += "\n";
    }
    std::cout << "printing input" << std::endl;
    std::cout << grid << std::endl;
  }

  return 0;
}
