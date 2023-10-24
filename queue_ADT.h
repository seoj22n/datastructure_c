#ifndef queue_ADT.h
#define queue_ADT.h

/// typedef
typedef struct node{
    void* dataPtr;
    struct node* link;
}QUEUE_NODE;

typedef struct{
    QUEUE_NODE* front;
    int count;
    QUEUE_NODE* rear;
}QUEUE;


///def prototype
QUEUE* createQueue(void);
QUEUE* destroyQueue(void);

int enqueue(QUEUE* queue, void* data);
int dequeue(QUEUE* queue, void** dataPtr);
int queueFront(QUEUE* queue, void** dataPtr);
int queueRear(QUEUE* queue, void** dataPtr);
int queueCount(QUEUE* queue);
int emptyQueue(QUEUE* queue);
int fullQueue(QUEUE* queue);


#endif