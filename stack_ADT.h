#ifndef stack_ADT.h
#define stack_ADT.h

/// stack typedef

typedef struct node{
    void* dataPtr;
    struct node* link;
}STACK_NODE;

typedef struct{
    int count;
    STACK_NODE* top;
}STACK;

///ADT function def
STACK* createStack(void);
int pushStack(STACK* stack, STACK_NODE* data);
void* popStack(STACK* stack);
void* retrieveStackTop(STACK* stack);
int emptyStack(STACK*stack);
int fullStack(STACK* stack);
int stackCount(STACK*stack);
STACK* destroyStacl(STACK* stack);

#endif