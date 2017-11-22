#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

int isprime(int n) {
  long long int i,squareroot;
  if (n>10) {
    squareroot = (long long int) sqrt(n);
    for (i=3; i<=squareroot; i=i+2)
      if ((n%i)==0)
        return 0;
    return 1;
  }
  else
    return 0;
}

int main(int argc, char *argv[])
{
  int pc,       /* prime counter */
      foundone, /* most recent prime found */
	  pcsum, /* prime sum*/
	  stride; /*Stride of number*/


  long long int n, limit,			
  				   maxprime; /* The max prime*/


  int ranks, /* Task ID */
  	  ntasks; /* Total number Tasks*/
  sscanf(argv[1],"%llu",&limit);	

  /* Init MPI */
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &ranks);
  MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
	  


  stride = ntasks*2;
  foundone = 0;
  pc = 0;
  if(ranks == 0){ /* Host */
      printf("Starting. Numbers to be scanned= %lld\n",limit);
	  pc=4;     /* Assume (2,3,5,7) are counted here */
	  for (n = 11; n <= limit; n = n+stride) {
		if (isprime(n)) {
		  pc++;
		  foundone = n;
		}			
	  }
	  MPI_Reduce(&pc, &pcsum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	  MPI_Reduce(&foundone, &maxprime, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
	  printf("Done. Largest prime is %d Total primes %d\n", maxprime, pcsum);
  }
  if(ranks > 0){ /*Slave*/
	  for (n = 11+ranks*2; n <= limit; n = n+stride) {
		if (isprime(n)) {
		  pc++;
		  foundone = n;
		}			
	  }
	  MPI_Reduce(&pc, &pcsum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	  MPI_Reduce(&foundone, &maxprime, 1, MPI_INT, MPI_MAX, 0,MPI_COMM_WORLD);
  }
  MPI_Finalize();
} 
