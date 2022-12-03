#include <pthread.h>

typedef struct DLLNode{
    int productID;
    pthread_mutex_t lock;
    struct DLLNode *next;
    struct DLLNode *prev;
}node;

typedef struct LinkedList{
 struct DLLNode *head;
 struct DLLNode *tail;
}list;

typedef struct InsertData{
    list *myList;
    // int data;
    int id;
    int N;
}insertData;

void *ThreadFunc(void *args);
void ListInsertInit(long id);
void ListPrint();
void ListThreadCreate();
void InitList();
void ListDelete(int key);
void ListInsertKey(int key);