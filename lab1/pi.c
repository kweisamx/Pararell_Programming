#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#define MAX_NUMBER 10000000

int global_sum= 0;
pthread_mutex_t mutex;


int calculate_pi(int n){
    int sum = 0;
    for(int i = 0; i < n;i++){
        double x = (double)rand()/RAND_MAX;   
        double y = (double)rand()/RAND_MAX;
        if ( x*x + y*y <= 1)
            sum+=1;
    }
    return sum;
}


void *thread_func(void *param){
    int step = *(int *)param;
    //printf("%d\n",step); //debug
    int s = calculate_pi(step);

    pthread_mutex_lock(&mutex); // lock
    global_sum += s;
    pthread_mutex_unlock(&mutex); //Unlock
}


int main(int argc, char **argv){
    
    // NOTE: It received two argument, one is the env CPU number, another is number of step
    if(argc != 3 ){
        printf("Please enter correct argument, ex, ./pi 4 10000");
        return 1;
    }
    int CPUNum =atoi(argv[1]);
    int run_step = atoi(argv[2]);
    int t_num = 4;
    pthread_t th[t_num];
    
    for(int i = 0; i < 4; i++){
        if(pthread_create(&th[i], NULL, thread_func, (void *)&run_step) != 0){
            perror("create thread failed");
        }
    }
    for(int j = 0; j < 4; j ++){
        pthread_join(th[j], NULL);
    }

    double pi = 4.0 * global_sum / (run_step * t_num) ;

    printf("%lf\n",pi);
    return 0;
}
