#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>

unsigned long long  global_sum= 0;
pthread_mutex_t mutex;

typedef struct thread_info{
    int pid;
    int step;
    int start;

}th_info;


void *thread_func(void *param){
    th_info t = *(th_info *)param;
    

    printf("i'm thread %d, %d ,%d\n",t.pid,t.step,t.start); //debug
    long long sum = 0;
    double x,y;
    unsigned int seed = time(NULL);

    for(int i = t.start; i < (t.step + t.start) ;i++){
        x = (double)rand_r(&seed)/RAND_MAX;
        y = (double)rand_r(&seed)/RAND_MAX;
        if ( x*x + y*y <= 1.0)
            sum++;
    }
    

    pthread_mutex_lock(&mutex); // lock
    global_sum += sum;
    pthread_mutex_unlock(&mutex); //Unlock

    pthread_exit(NULL);
}


int main(int argc, char **argv){
    
    // NOTE: It received two argument, one is the env CPU number, another is number of step
    if(argc != 3 ){
        printf("Please enter correct argument, ex, ./pi 4 10000");
        return 1;
    }

    int CPUNum = atoi(argv[1]);
    int run_step = atoi(argv[2]);
    time_t startwtime, endwtime;
    // The average of one thread need to run
    int step = run_step / CPUNum;

    // Create the thread
    pthread_t *th;
    th = (pthread_t *)malloc(CPUNum * sizeof(pthread_t));
    // CreateMutex
    pthread_mutex_init(&mutex, NULL);
    // Thread information
    th_info t_info[CPUNum];

    
    startwtime = time (NULL); 

    for(int i = 0; i < CPUNum; i++){
        t_info[i].pid = i;
        t_info[i].step = step;
        t_info[i].start = i * step ;

        if(pthread_create(&th[i], NULL, thread_func, (void *)&t_info[i]) != 0){
            perror("create thread failed");
        }
    }
    for(int j = 0; j < CPUNum; j ++){
        pthread_join(th[j], NULL);
    }
    
    pthread_mutex_destroy(&mutex);
    free(th);
    endwtime = time (NULL);
    printf ("wall clock time = %d\n", (endwtime - startwtime));
    printf("global_sum : %lld\n",global_sum);
    printf("pi rate : %lf\n",4.0 * (double)global_sum / (double)run_step);
    return 0;
}
