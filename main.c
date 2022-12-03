// #include <cstddef>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "list.h"
#include "hashTable.h"
#include <unistd.h>
#include "stack.h"

int prime_numbers[13] = {11 , 23 , 37 , 53 , 83 , 97 , 127 , 157 , 211 , 257 , 307 , 347 , 367};
int hashTable_size;
FILE *fptr;
int N;

int main(int argc , char* argv[]){

    N = atoi(argv[1]);
    if ( N > 120){
        perror("MAX NUMBER OF THREADS = 120\n");
        return 1;
    }
    printf("N = %d \n" , N);

    InitList();
    for (int l=0; l<13; l++){
        if(prime_numbers[l] > 3*N){
            hashTable_size = prime_numbers[l];
            break;
        }

    }
    InitHashTables(N/3 , hashTable_size);
    // InitStack();
    ListThreadCreate();
    // ListPrint();
    // HashTablePrint();
    // StackPrint();

    return 0;
}