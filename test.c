
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>


int main() {
    srand(time(NULL));

    for (int i=0; i<100; i++){
        int num = rand() % 100;
        printf("num = %d\n" , num);
    }

    return 0;
}