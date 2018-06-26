#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>


void * print_a(void *);
void * print_b(void *);
void * print_c(void *);
void * print_d(void *);
void * print_e(void *);

int main(){

    pthread_t t0;
    pthread_t t1;
    pthread_t t2;
    pthread_t t3;
    pthread_t t4;
   
    // 创建线程A
    if(pthread_create(&t0, NULL, print_a, NULL) == -1){
        puts("fail to create pthread t0");
        exit(1);
    }
    //创建线程B
    if(pthread_create(&t1, NULL, print_b, NULL) == -1){
        puts("fail to create pthread t1");
        exit(1);
    }	
    //创建线程C
    if(pthread_create(&t2,NULL,print_c,NULL) == -1){
	puts("fails to create pthread t2");
	exit(1);
   }
    //创建线程D
    if(pthread_create(&t3,NULL,print_d,NULL) == -1){
	puts("fails to create pthread t3");
	exit(1);
   }
    //创建线程C
    if(pthread_create(&t4,NULL,print_e,NULL) == -1){
	puts("fails to create pthread t4");
	exit(1);
   }

    // 等待线程结束
    void * result;
    if(pthread_join(t0, &result) == -1){
        puts("fail to recollect t0");
        exit(1);
    }

    if(pthread_join(t1, &result) == -1){
        puts("fail to recollect t1");
        exit(1);
    }

   if(pthread_join(t2,&result)==-1){
  	puts("fail to recollect t2");
	exit(1);
    }

   if(pthread_join(t3,&result)==-1){
  	puts("fail to recollect t3");
	exit(1);
    }

   if(pthread_join(t4,&result)==-1){
  	puts("fail to recollect t4");
	exit(1);
    }
    return 0;
}


// 线程A 方法
void * print_a(void *a){
        sleep(1);
        puts("Thread->1");
    return NULL;

}

// 线程B 方法
void * print_b(void *b){
        sleep(1);
        puts("Thread->2");
    return NULL;
}

//线程C 方法
void * print_c(void *c){
		sleep(1);
		puts("Thread->3");
	return NULL;
}

//线程D 方法
void * print_d(void *d){
		sleep(1);
		puts("Thread->4");
	return NULL;
}

//线程E 方法
void * print_e(void *e){
		sleep(1);
		puts("Thread->5");
	return NULL;
}
