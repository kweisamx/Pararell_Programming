#include<stdio.h>
#include<stdlib.h>
#include <omp.h>


void Test( int n ){
    for(int i = 0; i < 10000000; i++){
        int a = 0;
    }
   // printf("%d\n,", n);
}

int tid;

int main(int argc, char *argv[]){
    #pragma omp parallel 
    {
        #pragma omp for
        for(int i = 0; i < 10; i++){
        tid = omp_get_thread_num();
        printf("I'am thread %d \n",tid);
        }
    }
}
