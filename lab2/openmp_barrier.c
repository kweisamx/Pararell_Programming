#include<stdio.h>
#include<omp.h>

int main(){
    int tid;
    printf("I am the main thread. \n\n");
    #pragma omp parallel private(tid)
    {
        tid = omp_get_thread_num();
        printf("Thread %d: Hello. \n", tid);
        #pragma omp barrier
        printf("Thread %d: Bye bye. \n", tid);
    }
    
    printf("I am the main thread. \n\n");
    return 0;
}
