#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <errno.h>  
#include "shmdata.h"
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

#define MAX_TEXT 512  
struct msg_st  
{  
	long int msg_type;  
	char text[MAX_TEXT];  
}; 

#define MYPORT 8883
char* SERVERIP = "127.0.0.1";

#define ERR_EXIT(m) \
do { \
	perror(m); \
	exit(EXIT_FAILURE); \
} while (0)

//udp client
void echo_cli(int sock)
{
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(MYPORT);//设置端口
    servaddr.sin_addr.s_addr = inet_addr(SERVERIP);//设置服务器地址
    
    int ret;
    char sendbuf[1024] = {0};
    char recvbuf[1024] = {0};
    fgets(sendbuf, sizeof(sendbuf), stdin);
    

    printf("向服务器发送：%s\n",sendbuf);
    sendto(sock, sendbuf, strlen(sendbuf), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));

    ret = recvfrom(sock, recvbuf, sizeof(recvbuf), 0, NULL, NULL);
    if (ret == -1)
    {
    	if (errno == EINTR)
    		close(sock);
    	ERR_EXIT("recvfrom");
    }
    printf("从服务器接收：%s\n",recvbuf);

    memset(sendbuf, 0, sizeof(sendbuf));
    memset(recvbuf, 0, sizeof(recvbuf));
    
    
    close(sock);
    
}

void write_memory()
{
	int running = 1;
	void *shm = NULL;
	struct shared_use_st *shared = NULL;
	char buffer[BUFSIZ + 1];//用于保存输入的文本
	int shmid;
	//创建共享内存
	shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666|IPC_CREAT);
	if(shmid == -1)
	{
		fprintf(stderr, "shmget failed\n");
		exit(EXIT_FAILURE);
	}
	//将共享内存连接到当前进程的地址空间
	shm = shmat(shmid, (void*)0, 0);
	if(shm == (void*)-1)
	{
		fprintf(stderr, "shmat failed\n");
		exit(EXIT_FAILURE);
	}
	printf("Memory attached at %X\n", (int)shm);
	//设置共享内存
	shared = (struct shared_use_st*)shm;
	while(running)//向共享内存中写数据
	{
		//数据还没有被读取，则等待数据被读取,不能向共享内存中写入文本
		while(shared->written == 1)
		{
			sleep(1);
			printf("Waiting...\n");
		}
		//向共享内存中写入数据
		printf("Enter Memory text: ");
		fgets(buffer, BUFSIZ, stdin);
		strncpy(shared->text, buffer, TEXT_SZ);
		//写完数据，设置written使共享内存段可读
		shared->written = 1;
		//输入了end，退出循环（程序）
		if(strncmp(buffer, "end", 3) == 0)
			running = 0;
	}
	//把共享内存从当前进程中分离
	if(shmdt(shm) == -1)
	{
		fprintf(stderr, "shmdt failed\n");
		exit(EXIT_FAILURE);
	}

}


void write_msg()
{
	int send_running = 1;  
	struct msg_st data;  
    char buffer[BUFSIZ];  
	int msgid = -1;  
	
	msgid = msgget((key_t)1234, 0666 | IPC_CREAT);  
	if(msgid == -1)  
	{  
		fprintf(stderr, "msgget failed with error: %d\n", errno);  
		exit(EXIT_FAILURE);  
	}  

    //向消息队列中写消息，直到写入end  
	while(send_running)  
	{  
        //输入数据  
		printf("Enter Can text: ");  
		fgets(buffer, BUFSIZ, stdin);  
        data.msg_type = 1;    //注意2  
        strcpy(data.text, buffer);  
        //向队列发送数据  
        if(msgsnd(msgid, (void*)&data, MAX_TEXT, 0) == -1)  
        {  
        	fprintf(stderr, "msgsnd failed\n");  
        	exit(EXIT_FAILURE);  
        }  	

        //输入end结束输入  
        if(strncmp(buffer, "end", 3) == 0)  
        	send_running = 0;  
        sleep(1);  
    }  
}


int main()
{
	//写内存数据
	write_memory();
	//消息队列发送数据
	write_msg();

    int sock;
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    	ERR_EXIT("socket error");    
    echo_cli(sock);

    exit(EXIT_SUCCESS);
}


