#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>

int global_sum= 0;
pthread_mutex_t mutex;

typedef struct thread_info{
    int pid;
    int step;
    int start;

}th_info;

int calculate_pi(int n,int start){
    int sum = 0;
    double x,y;
    for(int i = start; i < n + start ;i++){
        x = (double)rand()/RAND_MAX;   
        y = (double)rand()/RAND_MAX;
        if ( x*x + y*y <= 1)
            sum+=1;
    }
    return sum;
}


void *thread_func(void *param){
    th_info t = *(th_info *)param;
    time_t startwtime, endwtime;
    

    startwtime = time (NULL); 
    printf("i'm thread %d, %d ,%d\n",t.pid,t.step,t.start); //debug
    int s = calculate_pi(t.step,t.start);
    endwtime = time (NULL);
    
    printf ("wall clock time = %d\n", (endwtime - startwtime));

    pthread_mutex_lock(&mutex); // lock
    global_sum += s;
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
    int t_num = 0;
    
    t_num = CPUNum;
    run_step = run_step / CPUNum;

    pthread_t th[t_num];
    th_info t_info[t_num];

    
    for(int i = 0; i < t_num; i++){
        t_info[i].pid = i;
        t_info[i].step = run_step;
        t_info[i].start = i * run_step ;

        if(pthread_create(&th[i], NULL, thread_func, (void *)&t_info[i]) != 0){
            perror("create thread failed");
        }
    }
    for(int j = 0; j < t_num; j ++){
        pthread_join(th[j], NULL);
    }

    double pi = 4.0 * global_sum / (run_step * t_num) ;

    printf("global_sum : %d\n",global_sum);
    printf("pi rate : %lf\n",pi);
    return 0;
}
