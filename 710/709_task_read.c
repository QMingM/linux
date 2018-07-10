#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/shm.h>
#include <errno.h>
#include "shmdata.h"
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define MYPORT 8883


#define ERR_EXIT(m) \
    do \
{ \
    perror(m); \
    exit(EXIT_FAILURE); \
    } while(0)

int n = 1;

struct msg_st  
{  
    long int msg_type;  
    char text[BUFSIZ];  
};
 //线程互斥量
pthread_mutex_t mutex;

pthread_cond_t cond;

//udp server
void echo_ser(int sock)
{

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(MYPORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    printf("监听%d端口\n",MYPORT);
    if (bind(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        ERR_EXIT("bind error");
    char recvbuf[1024] = {0};
    struct sockaddr_in peeraddr;
    socklen_t peerlen;
    int n;
    


        peerlen = sizeof(peeraddr);
        memset(recvbuf, 0, sizeof(recvbuf));
        n = recvfrom(sock, recvbuf, sizeof(recvbuf), 0,
           (struct sockaddr *)&peeraddr, &peerlen);
        if (n <= 0)
        {

            if (errno == EINTR)
                close(sock);
                // continue;
            
            ERR_EXIT("recvfrom error");
        }
        else if(n > 0)
        {
            printf("接收到的数据：%s\n",recvbuf);
            sendto(sock, recvbuf, n, 0,
             (struct sockaddr *)&peeraddr, peerlen);
            printf("回送的数据：%s\n",recvbuf);
        }
    
    close(sock);
}

//线程RTC函数
void *thread_RTC()
{

    pthread_mutex_lock(&mutex);
    while(n != 1)
    {
        pthread_cond_wait(&cond, &mutex);
    }

    printf("%d creat->thread_RTC->1\n", n);

    //开始读取内存数据   
    int running = 1;//程序是否继续运行的标志
    void *shm = NULL;//分配的共享内存的原始首地址
    struct shared_use_st *shared;//指向shm
    int shmid;//共享内存标识符 
    
    //创建共享内存
    shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666|IPC_CREAT);
    if(shmid == -1)
    {
        fprintf(stderr, "shmget failed\n");
        exit(EXIT_FAILURE);
    }
    //将共享内存连接到当前进程的地址空间
    shm = shmat(shmid, 0, 0);
    if(shm == (void*)-1)
    {
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
    }
    printf("\nMemory attached at %X\n", (int)shm);
    //设置共享内存
    shared = (struct shared_use_st*)shm;
    shared->written = 0;
    while(running)//读取共享内存中的数据
    {
        //没有进程向共享内存定数据有数据可读取
        if(shared->written != 0)
        {
            printf("You wrote memory: %s\n", shared->text);
            sleep(rand() % 3);
            //读取完数据，设置written使共享内存段可写
            shared->written = 0;
            //输入了end，退出循环（程序）
            if(strncmp(shared->text, "end", 3) == 0)
                running = 0;
        }
        else//有其他进程在写数据，不能读取数据
            sleep(1);
    }
    //把共享内存从当前进程中分离
    if(shmdt(shm) == -1)
    {
        fprintf(stderr, "shmdt failed\n");
        exit(EXIT_FAILURE);
    }
    //删除共享内存
    if(shmctl(shmid, IPC_RMID, 0) == -1)
    {
        fprintf(stderr, "shmctl(IPC_RMID) failed\n");
        exit(EXIT_FAILURE);
    }
    n++;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);


    if(n==1)
        pthread_exit(NULL);
    return NULL;
}

//线程CANDATE函数
void *thread_CANDATE()
{
    pthread_mutex_lock(&mutex);
    while(n  != 2)
    {
        pthread_cond_wait(&cond, &mutex);
    }
    printf("%d creat->thread_CANDATE->2\n", n);

    //接收消息队列的消息
    int receive_running = 1;
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
    while(receive_running)  
    {  
        if(msgrcv(msgid, (void*)&data, BUFSIZ, msgtype, 0) == -1)  
        {  
            fprintf(stderr, "msgrcv failed with errno: %d", errno);  
            exit(EXIT_FAILURE);  
        }  
        printf("You wrote can: %s\n",data.text);  
        //遇到end结束  
        if(strncmp(data.text, "end", 3) == 0)  
            receive_running = 0;  
    }  
    //删除消息队列  
    if(msgctl(msgid, IPC_RMID, 0) == -1)  
    {  
        fprintf(stderr, "msgctl(IPC_RMID) failed\n");  
        exit(EXIT_FAILURE);  
    }  

    n++;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);

    if(n==2)
        pthread_exit(NULL);
    return NULL;
}

//线程TIME函数
void *thread_TIME()
{
 pthread_mutex_lock(&mutex);
 while(n  != 3)
 {
     pthread_cond_wait(&cond, &mutex);
 }
 printf("%d creat->thread_TIME->3\n", n);
        //获取系统时间并打印
    time_t t;    //time_t是一种类型，定义time_t类型的t
    time(&t);    //取得当前时间
    printf("%s\n",ctime(&t));// ctime(&t)将日期转为字符串并打印
    n++;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);

    if(n==3)
        pthread_exit(NULL);
    return NULL;
}

    //线程UDP函数
void *thread_UDP()
{
 pthread_mutex_lock(&mutex);
 while(n  != 4)
 {
     pthread_cond_wait(&cond, &mutex);
 }
printf("%d creat->thread_UDP->4\n", n);

int sock;
//创建套接字描述符
if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    ERR_EXIT("socket");
    
echo_ser(sock);

 n++;
 pthread_cond_broadcast(&cond);
 pthread_mutex_unlock(&mutex);

 if(n==4)
    pthread_exit(NULL);
return NULL;
}

        //线程TCP函数
void *thread_TCP()
{
 pthread_mutex_lock(&mutex);
 while(n  != 5)
 {
     pthread_cond_wait(&cond, &mutex);
 }
 printf("%d creat->thread_TCP->5\n", n);

 n++;
 pthread_cond_broadcast(&cond);
 pthread_mutex_unlock(&mutex);

 if(n==5)
    pthread_exit(NULL);
return NULL;
}


int main(int argc, char** argv)
{
    pthread_t RTC,CANDATE,TIME,UDP,TCP;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);  

    if((pthread_create(&RTC,NULL,thread_RTC,NULL)) != 0)
        printf("pthread create error\n");
    if((pthread_create(&CANDATE,NULL,thread_CANDATE,NULL)) != 0)
        printf("pthread create error\n");
    if((pthread_create(&TIME,NULL,thread_TIME,NULL)) != 0)
        printf("pthread create error\n");
    if((pthread_create(&UDP,NULL,thread_UDP,NULL)) != 0)
        printf("pthread create error\n");
    if((pthread_create(&TCP,NULL,thread_TCP,NULL)) != 0)
        printf("pthread create error\n");

    pthread_join(RTC,NULL);
    pthread_join(CANDATE,NULL);
    pthread_join(TIME,NULL);
    pthread_join(UDP,NULL);
    pthread_join(TCP,NULL);


    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}


