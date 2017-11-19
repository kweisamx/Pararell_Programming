#include <stdio.h>
#include <math.h>
#include "mpi.h"

#define PI 3.1415926535

int main(int argc, char **argv) 
{
  long long i, num_intervals;
  double rect_width, area, sum, x_middle; 
  double totalsum;
  int ranks, ntasks;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &ranks);
  MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
  if(ranks == 0)
  	sscanf(argv[1],"%llu",&num_intervals);

  MPI_Bcast(&num_intervals, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);


  rect_width = PI / num_intervals;
  sum = 0; 
  totalsum = 0;

  for(i = 1+ranks; i < num_intervals + 1; i+=ntasks) {
	/* find the middle of the interval on the X-axis. */ 
	x_middle = (i - 0.5) * rect_width;
	sum += sin(x_middle) * rect_width; 
  } 
  MPI_Reduce(&sum, &totalsum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  if(ranks == 0)
  	printf("The total area is: %f\n", (float)totalsum);

  MPI_Finalize();
}   
