#include "list.h"
#include "hashTable.h"
#include "stack.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

// int thread_id=0;
pthread_barrier_t phase_barrier;
list *myList;
extern int N;
extern int hashTable_size;
extern FILE *fptr;
// int *ids;
void *ThreadFunc(void *args){

    long id = (long)args;
    /* -------------------------------- PHASE A START ------------------------------------------
       ------------------------------ INSERT TO THE LIST -------------------------------------*/
    ListInsertInit(id);
    pthread_barrier_wait(&phase_barrier);
    int listCnt=0;
    int listSum = 0;
    if (id == 0){
        printf("------------------ LIST CHEKCS ------------------------\n");
        node *tmp = myList->head->next;
        while(tmp != myList->tail){
            listCnt++;
            listSum += tmp->productID;
            tmp = tmp->next;
        }
        if (N*N == listCnt)
            printf("List Size check: expected %d and found %d \n" , N*N , listCnt);
        else{
            printf("List Size check FAILED\n");
            EXIT_FAILURE;
        }
        if ((N*N*(N*N-1))/2 == listSum)
            printf("List Sum chechk expected %d and found %d \n" , (N*N*(N*N-1))/2 , listSum);
        else{
            printf("List Sum check FAILED \n");
            EXIT_FAILURE;
        }
    }
    pthread_barrier_wait(&phase_barrier);
    /* ------------------------- PHASE A ENDS -------------------------------*/

    /* ----------------------------- PHASE B START ----------------------------
       -------------------------- DELETE FROM LIST ----------------------------
       ------------------------ INSTER TO HASH TABLE ------------------------*/
    int number_hashTables = N/3;
    int first = id * N;
    int last = first + N - 1;
    int index = id;
    for (int j=first; j<=last; j++){
        int hashTable = index % number_hashTables;   //in whick hashTable i am gona insert an item
        HTTInsert(hashTable , j);
        ListDelete(j);
        index++;
    }
    pthread_barrier_wait(&phase_barrier);
    if (id == 0){
        printf("--------------------- HASH TALES AFTER INSERT -------------------\n");
        checkHashCnt();
        checkHashSum();
    }
    pthread_barrier_wait(&phase_barrier); //try removing this barrier
    /* ----------------------- PHASE B ENDS --------------------------------------*/

    /* ------------------------- PHASE C START -----------------------------------
       --------------------- DELETE FROM HASH TABLE ---------------------------
       ---------------------- PUSH TO THE STACK ------------------------------*/
    index = id;
    int tmp = id;
    for(int j=0; j<N/3; j++){
        int key;
        int hashTable = index % number_hashTables;
        int flag = 0;
        do{
            key = tmp;
            flag = HTDelete(hashTable , key);
            tmp += number_hashTables;
        }while(flag == 0);
        push(key);
        index++;
    }
    pthread_barrier_wait(&phase_barrier);
    if (id == 0){
        printf("--------------------HASH TABLES AFTER DELETE --------------------\n");
        checkHashCntDelete();
        printf("------------------- STACK SIZE -----------------------\n");
        checkStackSize();
        printf("-------------------------------------------------------------------\n");
    }
    pthread_barrier_wait(&phase_barrier); 
    /* --------------------------- PHASE C ENDS ----------------------------------*/

    /* --------------------------- PHASE D START ----------------------------------
       ------------------------ POP FROM THE STACK -----------------------------
       ---------------------- INSERT BACK TO THE LIST --------------------------*/
    for (int i=0; i<N/3; i++){
        int key = pop();
        // printf("KEY = %d \n" , key);
        ListInsertKey(key);
    }
    pthread_barrier_wait(&phase_barrier);
    if (id == 0){
        node *tmp = myList->head->next;
        int cnt = 0;
        while(tmp != myList->tail){
            cnt++;
            tmp = tmp->next;
        }
        if (cnt == (N*N)/3)
            printf("Lisr size checked: expected %d , found %d \n" , (N*N)/3 , cnt);
        else{
            printf("Lisr size checked: expected: FAILED\n");
            printf("Lisr size checked: expected %d , found %d \n" , (N*N)/3 , cnt);
            EXIT_FAILURE;
        }
    }
    pthread_barrier_wait(&phase_barrier);
    /* ---------------------------- PHASE D ENDS ---------------------------------------*/

    return NULL;
}

void ListInsertInit(long id){
    node *pred, *curr;
    pred = myList->head;
    pthread_mutex_lock(&pred->lock);
    for(int i=0; i<N; i++){
        int data = i*N + id;    
        curr = pred->next;
        pthread_mutex_lock(&curr->lock);
        while (curr->productID < data) {
            node *tmp = pred;
            pred = curr;
            curr = curr->next;
            pthread_mutex_lock(&curr->lock);
            pthread_mutex_unlock(&tmp->lock);
        }
        if (data == curr->productID) {
            pthread_mutex_unlock(&pred->lock);
            pthread_mutex_unlock(&curr->lock);
            return;
        }
        else {
            node *new_node = (node*)malloc(sizeof(node));
            pthread_mutex_init(&(new_node->lock), NULL);
            new_node->next = curr;
            new_node->prev = pred;
            new_node->productID = data;
            pred->next = new_node;
            curr->prev = new_node;
            pthread_mutex_unlock(&pred->lock);
            pthread_mutex_unlock(&curr->lock);
        }
    }
}

void ListInsertKey(int key){
    node *pred, *curr;
    pred = myList->head;
    pthread_mutex_lock(&pred->lock);
    curr = pred->next;
    pthread_mutex_lock(&curr->lock);
    while (curr->productID < key) {
        node *tmp = pred;
        pred = curr;
        curr = curr->next;
        pthread_mutex_lock(&curr->lock);
        pthread_mutex_unlock(&tmp->lock);
    }
    if (key == curr->productID) {
        printf("R\n");
        pthread_mutex_unlock(&pred->lock);
        pthread_mutex_unlock(&curr->lock);
        return;
    }
    else {
        node *new_node = (node*)malloc(sizeof(node));
        pthread_mutex_init(&(new_node->lock), NULL);
        new_node->next = curr;
        new_node->prev = pred;
        new_node->productID = key;
        pred->next = new_node;
        curr->prev = new_node;
        pthread_mutex_unlock(&pred->lock);
        pthread_mutex_unlock(&curr->lock);
    }
}

void ListPrint(){
    node *head = myList->head;
    printf("----------------- PRINT LIST --------------------\n");
    while (head != NULL){
        printf("node = %d \n", head->productID);
        head = head->next;
    }
    printf("head = %d \n" , myList->head->productID);
    printf("tail = %d \n" , myList->tail->productID);
}

void InitList(){
    myList = (list*)malloc(sizeof(list));
    myList->head = (node*)malloc(sizeof(node));
    myList->tail = (node*)malloc(sizeof(node));
    myList->head->productID = INT_MIN;
    myList->head->next = myList->tail;
    myList->head->prev = NULL;
    pthread_mutex_init(&(myList->head->lock), NULL);
    myList->tail->productID = INT_MAX;
    myList->tail->next = NULL;
    myList->tail->prev = myList->head;
    pthread_mutex_init(&(myList->tail->lock), NULL);
}

void ListThreadCreate(){
    // N = number_threads;
    long ids[N];
    pthread_barrier_init (&phase_barrier, NULL, N);
    pthread_t my_threads[N];
    int i;
    srand(time(NULL));
    for(i=0; i<N; i++){
        ids[i] = i;
        if (pthread_create(&my_threads[i], NULL, &ThreadFunc, (void*)ids[i]) != 0) {
                perror("Failed to create thread");
        }
    }

    for (i=0; i<N; i++) {
        if (pthread_join(my_threads[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }

    pthread_barrier_destroy(&phase_barrier);
}

void ListDelete(int key){
    pthread_mutex_lock(&myList->head->lock);
    node *pred, *curr , *next;
    pred = myList->head;
    curr = pred->next;
    if (curr == myList->tail){
        pthread_mutex_unlock(&myList->head->lock);
        printf("The list is null \n");
        return;
    }
    pthread_mutex_lock(&curr->lock);
    next = curr->next;
    pthread_mutex_lock(&next->lock);
    while (curr->productID < key) {
        // printf("1");
        pthread_mutex_unlock(&pred->lock);
        pred = curr;
        curr = next;
        next = curr->next;
        pthread_mutex_lock(&next->lock);
    }
    // printf("2");
    if (key != curr->productID) {
        // printf("R");
        pthread_mutex_unlock(&pred->lock);
        pthread_mutex_unlock(&curr->lock);
        pthread_mutex_unlock(&next->lock);
        printf("\n");
        return ;
    }
    else {
        pred->next = next;
        next->prev = pred;
    }
    pthread_mutex_unlock(&pred->lock);
    pthread_mutex_unlock(&curr->lock);
    pthread_mutex_unlock(&next->lock);
    // free(args);
}