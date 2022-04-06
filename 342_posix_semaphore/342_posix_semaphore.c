#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>


#define NUM_THREAD  10
#define NUM_LOOP 10
#define SEMAPHORE_NAME "/mysemaphore"

int count = 0;


void *th(void *arg)
{
    int *threadNum = (int *)arg;
    printf("[TH=%2d] Start\n",*threadNum);
    sem_t *s = sem_open(SEMAPHORE_NAME,0);
    if(s == SEM_FAILED)
    {
        perror("sem_open");
        return NULL;
    }
    
    for(int i = 0;i < NUM_LOOP; i++)
    {
        if(sem_wait(s) == -1)
        {
            perror("sem_wait");
            return NULL;
        }
        
        __sync_fetch_and_add(&count,1);
        printf("[TH=%2d] Run , count = %d\n",*threadNum,count);
        
        // 10ms sleep
        usleep(10000);
        
        
        __sync_fetch_and_sub(&count,1);
        
        if(sem_post(s) == -1)
        {
            perror("sem_post");
            return NULL;
        }
        
        // usleep(10);
    }
    
    if(sem_close(s) == -1)
    {
        perror("sem_close");
    }
    printf("[TH=%2d] Fin\n",*threadNum);
    
    return NULL;
}

int main(int argc , char *argv[])
{
    perror("main start");
    sem_t *s = sem_open(SEMAPHORE_NAME, O_CREAT,0660,3);
    if( s == SEM_FAILED)
    {
        perror("sem_open");
        return -1;
    }
    
    pthread_t v[NUM_THREAD];
    int threadNumberArray[NUM_THREAD] = {0};
    for(int i = 0;i < NUM_THREAD; i++)
    {
        threadNumberArray[i] = i + 1;
        printf("[TH=%2d] Create\n",i+1);
        pthread_create(&v[i],NULL,th,(void *)&threadNumberArray[i]);
    }
    
    for(int i = 0;i < NUM_THREAD;i++)
    {
        pthread_join(v[i],NULL);
    }
    
    if(sem_close(s) == -1)
    {
        perror("sem_close");
    }
    
    if(sem_unlink(SEMAPHORE_NAME) == -1)
    {
        perror("sem_unlink");
    }
    
    return 0;
}
