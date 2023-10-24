#include <stdio.h>
#include "stack_ADT.h"


STACK* createStack(void){ //stack을 만든다는건 meta data, head를 만든다는 것.
    STACK* newStack = (STACK*)malloc(sizeof(STACK));
    
    if(newStack == NULL)    return 0;

    newStack->count = 0;
    newStack->top = NULL;

    return newStack;
}

int pushStack(STACK* stack, STACK_NODE* data){ // newNode 만들어서 데이터를 담고, 그 이후 stack에 삽입
    STACK_NODE* newNode = (STACK_NODE*)malloc(sizeof(STACK_NODE));
    if(newNode == NULL)    return 0;

    newNode->dataPtr = data;
    
    newNode->link = stack->top;
    stack->top = newNode;

    stack->count ++;
    return 1;
}

void* popStack(STACK* stack){ // 가장 위에 있는 node pointer 출력 후 내부적으론 해당 노드 삭제
    
    if(stack->count == 0)    return NULL;

    void* outData;
    outData = stack->top->dataPtr;

    stack->top = stack->top->link;
    free(stack->top);

    stack->count--;

    return outData;

}

void* retrieveStackTop(STACK* stack){

    if(stack->count == 0)    return NULL;
    void* dataOut;
    dataOut = stack->top;
    return dataOut;
}

int emptyStack(STACK*stack){
    if(stack->count == 0) return 1;
    else return 0;
}

int fullStack(STACK* stack){
    STACK_NODE* new = (STACK_NODE*)malloc(sizeof(STACK_NODE));
    if (new == NULL)    return 1;
    else return 0;
}

int stackCount(STACK*stack){
    return stack->count;
}

STACK* destroyStacl(STACK* stack){
    STACK_NODE* temp;

    while(stack->top != NULL){
        free(stack->top->dataPtr);
        temp = stack->top;
        stack->top = temp->link;

        free(temp);
    }

    free(stack);
    return NULL;
}
