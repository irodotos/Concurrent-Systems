#include <pthread.h>


typedef struct HTNode{ 
      int productID; 
      pthread_mutex_t lock; 
}hashItem;

typedef struct hashTable{
    hashItem* items;
    int size;
}hashTable;

void InitHashTables(int number_hashTables , int size);
void HTTInsert(int hashTable , int key);
int HTDelete(int hashTable , int key);
void HashTablePrint();
void checkHashCnt();
void checkHashSum();
void checkHashCntDelete();