#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>


int main(){
    for (int i = 0; i < 10; ++i)
    {
       printf("%d\n", i%5);
    }
   
    return 0;
}

