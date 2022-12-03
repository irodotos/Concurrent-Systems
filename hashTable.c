#include "hashTable.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

hashTable *hashTables;
int num_hash;
int ssize;
extern FILE *fptr;
extern int N;

void InitHashTables(int number_hashTables , int size){
    printf("number hashTbales = %d \n" , number_hashTables);
    hashTables = (hashTable*)malloc(number_hashTables *sizeof(hashTable));
    num_hash = number_hashTables;
    ssize = size;
    for (int i=0; i<number_hashTables; i++){
        hashTables[i].items = (hashItem*)malloc(size * sizeof(hashItem));
        hashTables[i].size = size;
        for (int j=0; j<size; j++){
            hashTables[i].items[j].productID = -1;
            pthread_mutex_init(&(hashTables[i].items[j].lock), NULL);
        }
    }
}

int hash1(int key){
    return key % ssize;
}

int hash2(int key , int i){
    return (hash1(key) + i ) % ssize;
}

void HTTInsert(int hashTable , int key){
    int curr = hash1(key);
    pthread_mutex_lock(&hashTables[hashTable].items[curr].lock);
    if (hashTables[hashTable].items[curr].productID == -1){
        hashTables[hashTable].items[curr].productID = key;
        pthread_mutex_unlock(&hashTables[hashTable].items[curr].lock);
        return;
    }
    else{
        pthread_mutex_unlock(&hashTables[hashTable].items[curr].lock);
        curr = hash2(key , 1);
        pthread_mutex_lock(&hashTables[hashTable].items[curr].lock);
        for (int i=1; i<=ssize; i++){
            if (hashTables[hashTable].items[curr].productID !=  -1){
                if (hashTables[hashTable].items[curr].productID > key){
                    int tmp = key;
                    key = hashTables[hashTable].items[curr].productID;
                    hashTables[hashTable].items[curr].productID = tmp;
                }
                pthread_mutex_unlock(&hashTables[hashTable].items[curr].lock);
                curr = hash2(key , i+1);
                pthread_mutex_lock(&hashTables[hashTable].items[curr].lock);
            }
            else{
                hashTables[hashTable].items[curr].productID = key;
                pthread_mutex_unlock(&hashTables[hashTable].items[curr].lock);
                return;
            }
        }
    }
}

int HTDelete(int hashTable , int key){
    int curr = hash1(key);
    pthread_mutex_lock(&hashTables[hashTable].items[curr].lock);
    if(hashTables[hashTable].items[curr].productID == key){
        hashTables[hashTable].items[curr].productID = -1;
        pthread_mutex_unlock(&hashTables[hashTable].items[curr].lock);
        return 1;
    }
    pthread_mutex_unlock(&hashTables[hashTable].items[curr].lock);
    curr = hash2(key, 1);
    pthread_mutex_lock(&hashTables[hashTable].items[curr].lock);
    for(int i=1; i<=ssize; i++){
        if(hashTables[hashTable].items[curr].productID != key){
            pthread_mutex_unlock(&hashTables[hashTable].items[curr].lock);
            curr = hash2(key , i+1);
            pthread_mutex_lock(&hashTables[hashTable].items[curr].lock);
        }
        else{
            hashTables[hashTable].items[curr].productID = -1;
            pthread_mutex_unlock(&hashTables[hashTable].items[curr].lock);
            return 1;
        }
    }
    pthread_mutex_unlock(&hashTables[hashTable].items[curr].lock);
    return 0;
}

void HashTablePrint(){
    for(int i=0; i<num_hash; i++){
        printf("PINAKAS %d \n" , i);
        for (int j=0; j<ssize; j++){
            printf("items = %d \n" , hashTables[i].items[j].productID);
        }
    }
}

void checkHashSum(){
    int sum = 0;
    int n = 3*num_hash;
    for(int i=0; i<num_hash; i++){
        for (int j=0; j<ssize; j++){
            if (hashTables[i].items[j].productID == -1)
                continue;
            sum += hashTables[i].items[j].productID;
        }
        
    }
    if (sum ==  (n*n*(n*n-1))/2)
        printf("HT[] sum check: expected %d , found %d\n"  , (n*n*(n*n-1))/2 , sum);
    else{
        printf("HT[] sum check FAILED\n");
        printf("expected = %d , found = %d \n" , (n*n*(n*n-1))/2 , sum);
        EXIT_FAILURE;
    }
}

void checkHashCnt(){
    int cnt;
    int n = 3*num_hash;
    for(int i=0; i<num_hash; i++){
        cnt = 0;
        for (int j=0; j<ssize; j++){
            if (hashTables[i].items[j].productID == -1)
                continue;
            cnt++;
        }
        if (cnt == 3*n)
            printf("HT[%d] size check: expected %d , found %d\n" , i , 3*n , cnt);
        else{
            printf("HT[%d] size check FAILED\n" , i);
            printf("HT[%d] size check: expected %d , found %d\n" , i , 3*n , cnt);
            EXIT_FAILURE;
        }
    }
}

void checkHashCntDelete(){
    int cnt;
    int n = 3*num_hash;
    for(int i=0; i<num_hash; i++){
        cnt = 0;
        for (int j=0; j<ssize; j++){
            if (hashTables[i].items[j].productID == -1)
                continue;
            cnt++;
        }
        if (cnt == 2*n)
            printf("HT[%d] size check: expected %d , found %d\n" , i , 2*n , cnt);
        else{
            printf("HT[%d] size check FAILED\n" , i);
            printf("HT[%d] size check: expected %d , found %d\n" , i , 2*n , cnt);
            EXIT_FAILURE;
        }
    }
}
