#include "stack.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

stackItem *top;
pthread_mutex_t mutex;
extern int N;
int COUNTER = 0;

void InitStack(){
    top = (stackItem*)malloc(sizeof(stackItem));
}

int tryPush(stackItem *item){
    pthread_mutex_lock(&mutex);
    item->next = top;
    top = item;
    pthread_mutex_unlock(&mutex);
    return 1;
}

void push(int key){
    stackItem *newNode = (stackItem*)malloc(sizeof(stackItem));
    newNode->productID = key;
    while(1){
        if(tryPush(newNode))return;
        // else sleep(rand()%4);
    }
}

stackItem* tryPop(){
    pthread_mutex_lock(&mutex);
    stackItem *oldTop = top;
    stackItem *newTop;
    if(oldTop == NULL){
        printf("TRY TO POP FROM EMPTY STACK\n");
        return NULL;
    }
    // newTop = oldTop->next;
    COUNTER++;
    stackItem *tmp = oldTop;
    // oldTop = oldTop->next;
    top = top->next;
    pthread_mutex_unlock(&mutex);
    return tmp;
}

int pop(void){
    stackItem *rn;
    while(1){
        rn = tryPop();
        if(rn == NULL) {
            printf("TRY TO POP FROM EMPTY STACK \n");
            EXIT_FAILURE;
        }
        else return rn->productID;
    }
}

void StackPrint(){
    stackItem *tmp = top;
    while(tmp != NULL){
        printf("stack = %d \n" , tmp->productID);
        tmp = tmp->next;
    }
    // printf("COUNTER = %d\n" , COUNTER);
    // printf("top of stack = %d \n" , top->productID);
}

void checkStackSize(){
    stackItem *tmp = top;
    int cnt = 0;
    while(tmp != NULL){
        cnt++;
        tmp = tmp->next;
    }
    if (cnt == (N*N)/3)
        printf("Stack size checked: expected %d , found %d \n" , (N*N)/3 , cnt);
    else{
        printf("Stack size checked: FAILED\n");
        printf("Stack size checked: expected %d , found %d \n" , (N*N)/3 , cnt);
        EXIT_FAILURE;
    }
     
}