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
    #pragma omp parallel private(tid)
    for(int i = 0; i < 10; i++){
    tid = omp_get_thread_num();
    printf("i'm tid :%d,i enter\n",tid);
        
        Test(i);
    printf("i'm tid :%d,i finish\n",tid);
    }
}
