#include <unistd.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <string.h>  
#include <errno.h>  
#include <sys/msg.h>
#include <unistd.h>  
#include <pthread.h>
  
struct msg_st  
{  
    long int msg_type;  
    char text[BUFSIZ];  
};  

void * print_a(void *);
void * print_b(void *);
void * print_c(void *);
void * print_d(void *);
void * print_e(void *);
  
int main()  
{  
    pthread_t t0;
    pthread_t t1;
    pthread_t t2;
    pthread_t t3;
    pthread_t t4;
   
    int running = 1;  
    int msgid = -1;  
    struct msg_st data;  
    long int msgtype = 0; //注意1  
  
    //建立消息队列  
    msgid = msgget((key_t)1234, 0666 | IPC_CREAT);  
    if(msgid == -1)  
    {  
        fprintf(stderr, "msgget failed with error: %d\n", errno);  
        exit(EXIT_FAILURE);  
    }  
    //从队列中获取消息，直到遇到end消息为止  
    while(running)  
    {  
        if(msgrcv(msgid, (void*)&data, BUFSIZ, msgtype, 0) == -1)  
        {  
            fprintf(stderr, "msgrcv failed with errno: %d\n", errno);  
            exit(EXIT_FAILURE);  
        }  
  
	//创建线程0
       if(pthread_create(&t0,NULL,print_a,NULL)==-1){
	    puts("fail to create pthread t0");
	    exit(1);
        }
	//创建线程0
       if(pthread_create(&t1,NULL,print_b,NULL)==-1){
	    puts("fail to create pthread t1");
	    exit(1);
        }
	//创建线程0
       if(pthread_create(&t2,NULL,print_c,NULL)==-1){
	    puts("fail to create pthread t2");
	    exit(1);
        }
	//创建线程0
       if(pthread_create(&t3,NULL,print_d,NULL)==-1){
	    puts("fail to create pthread t3");
	    exit(1);
        }
	//创建线程0
       if(pthread_create(&t4,NULL,print_e,NULL)==-1){
	    puts("fail to create pthread t4");
	    exit(1);
        }

        printf("You wrote: %s\n",data.text);  
        //遇到end结束  
        if(strncmp(data.text, "end", 3) == 0)  
            running = 0;  
    }  
    //删除消息队列  
    if(msgctl(msgid, IPC_RMID, 0) == -1)  
    {  
        fprintf(stderr, "msgctl(IPC_RMID) failed\n");  
        exit(EXIT_FAILURE);  
    }  
    exit(EXIT_SUCCESS);  
} 

//线程0方法
void * print_a(void *a){
	sleep(1);
	puts("Thread->1");
	return NULL;
}
void * print_b(void *a){
	sleep(1);
	puts("Thread->2");
	return NULL;
}
void * print_c(void *a){
	sleep(1);
	puts("Thread->3");
	return NULL;
}
void * print_d(void *a){
	sleep(1);
	puts("Thread->4");
	return NULL;
}
void * print_e(void *a){
	sleep(1);
	puts("Thread->5");
	return NULL;
}
