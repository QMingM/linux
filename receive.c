/*receive.c */  
 2 #include <stdio.h>   
 3 #include <sys/types.h>   
 4 #include <sys/ipc.h>   
 5 #include <sys/msg.h>   
 6 #include <errno.h>   
 7   
 8 #define MSGKEY 1024   
 9   
10 struct msgstru  
11 {  
12    long msgtype;  
13    char msgtext[2048];  
14 };  
15   
16 /*子进程，监听消息队列*/  
17 void childproc(){  
18   struct msgstru msgs;  
19   int msgid,ret_value;  
20   char str[512];  
21     
22   while(1){  
23      msgid = msgget(MSGKEY,IPC_EXCL );/*检查消息队列是否存在 */  
24      if(msgid < 0){  
25         printf("msq not existed! errno=%d [%s]\n",errno,strerror(errno));  
26         sleep(2);  
27         continue;  
28      }  
29      /*接收消息队列*/  
30      ret_value = msgrcv(msgid,&msgs,sizeof(struct msgstru),0,0);  
31      printf("text=[%s] pid=[%d]\n",msgs.msgtext,getpid());  
32   }  
33   return;  
34 }  
35   
36 void main()  
37 {  
38   int i,cpid;  
39   
40   /* create 5 child process */  
41   for (i=0;i<5;i++){  
42      cpid = fork();  
43      if (cpid < 0)  
44         printf("fork failed\n");  
45      else if (cpid ==0) /*child process*/  
46         childproc();  
47   }  
48 }  
