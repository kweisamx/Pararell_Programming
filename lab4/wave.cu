
/**********************************************************************
 * DESCRIPTION:
 *   Serial Concurrent Wave Equation - C Version
 *   This program implements the concurrent wave equation
 *********************************************************************/
#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MAXPOINTS 1000000
#define MAXSTEPS 1000000
#define MINPOINTS 20
#define PI 3.14159265

void check_param(void);
void init_line(void);
void update (void);
void printfinal (void);

int nsteps,                 	/* number of time steps */
    tpoints, 	     		/* total points along string */
    rcode;                  	/* generic return code */
float  values[MAXPOINTS+2], 	/* values at time t */
       oldval[MAXPOINTS+2], 	/* values at time (t-dt) */
       newval[MAXPOINTS+2]; 	/* values at time (t+dt) */
float *gvalue, *goldval, *gnewval;

int size = MAXPOINTS+2;

/**********************************************************************
 *	Checks input values from parameters
 *********************************************************************/
void check_param(void)
{
   char tchar[20];

   /* check number of points, number of iterations */
   while ((tpoints < MINPOINTS) || (tpoints > MAXPOINTS)) {
      printf("Enter number of points along vibrating string [%d-%d]: "
           ,MINPOINTS, MAXPOINTS);
      scanf("%s", tchar);
      tpoints = atoi(tchar);
      if ((tpoints < MINPOINTS) || (tpoints > MAXPOINTS))
         printf("Invalid. Please enter value between %d and %d\n", 
                 MINPOINTS, MAXPOINTS);
   }
   while ((nsteps < 1) || (nsteps > MAXSTEPS)) {
      printf("Enter number of time steps [1-%d]: ", MAXSTEPS);
      scanf("%s", tchar);
      nsteps = atoi(tchar);
      if ((nsteps < 1) || (nsteps > MAXSTEPS))
         printf("Invalid. Please enter value between 1 and %d\n", MAXSTEPS);
   }

   printf("Using points = %d, steps = %d\n", tpoints, nsteps);

}


__global__ void gpu_init_old_val(float *a, float *b, float *c, int n){
        int j=blockIdx.x*blockDim.x+threadIdx.x;
        int m=gridDim.x*blockDim.x;
        for(int k=j; k<n; k+=m){
            a[k] = b[k];
        }
        __syncthreads();
}
__global__ void gpu_update(float *a, float *b, float *c, int n){
        int j=blockIdx.x*blockDim.x+threadIdx.x;
        int m=gridDim.x*blockDim.x;
        for(int k=j; k<n; k+=m){
            a[k] = b[k];
            b[k] = c[k];
        }
        __syncthreads();
        
}
__global__ void gpu_update_point(float *a, float *b, float *c, int n){
        for(int k=0; k<n; k++){
            a[k] = b[k];
            b[k] = c[k];
        }
        __syncthreads();
}

/**********************************************************************
 *      Calculate new values using wave equation
 *********************************************************************/
void do_math(int i)
{
   float dtime, c, dx, tau, sqtau;

   dtime = 0.3;
   c = 1.0;
   dx = 1.0;
   tau = (c * dtime / dx);
   sqtau = tau * tau;
   newval[i] = (2.0 * values[i]) - oldval[i] + (sqtau *  (-2.0)*values[i]);
}



/**********************************************************************
 *     Initialize points on line
 *********************************************************************/
void init_line(void)
{
   int i, j;
   float x, fac, k, tmp;

   /* Calculate initial values based on sine curve */
   fac = 2.0 * PI;
   k = 0.0; 
   tmp = tpoints - 1;
   for (j = 1; j <= tpoints; j++) {
      x = k/tmp;
      values[j] = sin (fac * x);
      k = k + 1.0;
   } 
   cudaMemcpy(gvalue, values, size, cudaMemcpyHostToDevice);
   cudaMemcpy(goldval, oldval, size, cudaMemcpyHostToDevice);
   cudaMemcpy(gnewval, newval, size, cudaMemcpyHostToDevice);
   
   /* Initialize old values array */
   gpu_init_old_val<<<30,512>>>(goldval, gvalue, gnewval, tpoints);

   cudaMemcpy(values, gvalue, size, cudaMemcpyDeviceToHost);
   cudaMemcpy(oldval, goldval, size, cudaMemcpyDeviceToHost);

   printf("Updating all points for all time steps...\n");
   
   /* Update values for each time step */
   for (i = 1; i<= nsteps; i++) {
      /* Update points along line for this time step */
      for (j = 1; j <= tpoints; j++) {
         if ((j == 1) || (j  == tpoints))
            newval[j] = 0.0;
         else
            do_math(j);
      }

      /* Update old values with new values */
      
      for (j = 1; j <= tpoints; j++) {
         oldval[j] = values[j];
         values[j] = newval[j];
      }

      //gpu_update<<<1,1>>>(goldval, gvalue, newval, tpoints);
      //cudaMemcpy(newval, gnewval, size, cudaMemcpyDeviceToHost);
      //cudaMemcpy(values, gvalue, size, cudaMemcpyDeviceToHost);
   }

}

/**********************************************************************
 *     Update all values along line a specified number of times
 *********************************************************************/

/**********************************************************************
 *     Print final results
 *********************************************************************/
void printfinal()
{
   int i;

   for (i = 1; i <= tpoints; i++) {
      printf("%6.4f ", values[i]);
      if (i%10 == 0)
         printf("\n");
   }
}


/**********************************************************************
 *	Main program
 *********************************************************************/
int main(int argc, char *argv[])
{
	sscanf(argv[1],"%d",&tpoints);
	sscanf(argv[2],"%d",&nsteps);
	check_param();

    // set GPU memory 
    cudaMalloc((void**)&gvalue, size);
    cudaMalloc((void**)&goldval, size);
    cudaMalloc((void**)&gnewval, size);

	printf("Initializing points on the line...\n");
	init_line();
	printf("Printing final results...\n");
	printfinal();
	printf("\nDone.\n\n");
	
	return 0;
}
