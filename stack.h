#include <pthread.h>

typedef struct stackNode{ 
      int productID; 
      struct stackNode *next; 
}stackItem;

void InitStack();
void push(int key);
int pop(void);
void StackPrint();
void checkStackSize();