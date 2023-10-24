#include <stdio.h>
#include "queue_ADT.h"

QUEUE* createQueue(void){
    QUEUE* newQueue = (QUEUE*)malloc(sizeof(QUEUE));
    if(newQueue == NULL) return NULL;
    newQueue->count = 0;
    newQueue->front = NULL;
    newQueue->rear = NULL;

    return newQueue;
}

QUEUE* destroyQueue(QUEUE* queue){
    QUEUE_NODE*delNode;
    while(queue->front != 0){
        delNode = queue->front;
        queue->front = queue->front->link;
        free(delNode);
    }
    free(queue);
    return delNode;
}

int enqueue(QUEUE* queue, void* data){
    QUEUE_NODE* newNode = (QUEUE_NODE*)malloc(sizeof(QUEUE_NODE));
   

    newNode->dataPtr = data;
    newNode->link = NULL;

    if(queue->count == 0){
        queue->front = newNode;
        queue->rear = newNode;
    }

    else{
    queue->rear->link = newNode;
    queue->rear = newNode;
    }

    queue->count ++;

    return 1;
}

int dequeue(QUEUE* queue, void** dataPtr){
    *dataPtr = queue->rear->dataPtr; // 삭제할 데이터가 뭔지 담아둠.. pop은 아니므로 명시적으로 return 하진 않지만 이중포인터써서 뭐가 삭제됐는지 함수 외부에서 확인할 수 있도록
    if(queue->count == 0)   return 0;

    if(queue->count == 1){
        queue->front = NULL;
        queue->rear = NULL;
    }
    else{
        queue->front = queue->front->link;
    }
    free(queue->front);
    queue->count --;
    return 1;
}

int queueFront(QUEUE* queue, void** dataPtr){
    if(queue->count ==0)    return 0;
    *dataPtr = queue->front->dataPtr;
    return 1;
}

int queueRear(QUEUE* queue, void** dataPtr){
    if(queue->count == 0) return 0;
    *dataPtr = queue->rear->dataPtr;
    return 1;
}

int queueCount(QUEUE* queue){
    return queue->count;
}

int emptyQueue(QUEUE* queue){
    if(queue->count == 0) return 1;
    else return 0;
}

int fullQueue(QUEUE* queue){
    QUEUE_NODE*new = (QUEUE_NODE*)malloc(sizeof(QUEUE_NODE));
    if(new == NULL) return 1;
    else return 0;
}
