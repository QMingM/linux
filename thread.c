#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
 
int n = 1;
int N = 100;
 
 //线程互斥量
pthread_mutex_t mutex;

pthread_cond_t cond;
 
void *func1()
{
    while(n<=N)
    {

        pthread_mutex_lock(&mutex);
        while(n % 5  != 4)
        {
                pthread_cond_wait(&cond, &mutex);
        }

        printf("%d thread->1\n", n);
        n++;
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);
    }
     
    if(n==N)
        pthread_exit(NULL);
    return NULL;
}
 
void *func2()
{
    while(n<=N)
    {

        pthread_mutex_lock(&mutex);
         while(n % 5 != 0)
        {
                pthread_cond_wait(&cond, &mutex);
        }
        printf("%d thread->2\n", n);
        n++;
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);
    }
    if(n==N)
        pthread_exit(NULL);
    return NULL;
}

void *func3()
{
    while(n<=N)
    {
         pthread_mutex_lock(&mutex);
         while(n % 5 != 1)
        {
                pthread_cond_wait(&cond, &mutex);
        }
        printf("%d thread->3\n", n);
        n++;
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);
    }
    if(n==N)
        pthread_exit(NULL);
    return NULL;
}

void *func4()
{
    while(n<=N)
    {
        pthread_mutex_lock(&mutex);
         while(n % 5 != 2)
        {
                pthread_cond_wait(&cond, &mutex);
        }
        printf("%d thread->4\n", n);
        n++;
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);
    }
    if(n==N)
        pthread_exit(NULL);
    return NULL;
}

void *func5()
{
    while(n<=N)
    {
        pthread_mutex_lock(&mutex);
         while(n % 5 != 3)
        {
                pthread_cond_wait(&cond, &mutex);
        }
        printf("%d thread->5\n", n);

        n++;
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);
    }
    if(n==N)
        pthread_exit(NULL);
    return NULL;
}
 
int main(int argc, char** argv)
{
    pthread_t pid1,pid2,pid3,pid4,pid5;
 
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);  
     
    if((pthread_create(&pid1,NULL,func1,NULL)) != 0)
        printf("pthread create error\n");
    if((pthread_create(&pid2,NULL,func2,NULL)) != 0)
        printf("pthread create error\n");
    if((pthread_create(&pid3,NULL,func3,NULL)) != 0)
        printf("pthread create error\n");
    if((pthread_create(&pid4,NULL,func4,NULL)) != 0)
        printf("pthread create error\n");
    if((pthread_create(&pid5,NULL,func5,NULL)) != 0)
        printf("pthread create error\n");
     
    pthread_join(pid1,NULL);
    pthread_join(pid2,NULL);
    pthread_join(pid3,NULL);
    pthread_join(pid4,NULL);
    pthread_join(pid5,NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
     
    return 0;
}