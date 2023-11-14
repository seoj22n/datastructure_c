#include <stdlib.h> // malloc

#include "adt_dlist.h"

// internal insert function
// inserts data into list
// for addNode function
// return	1 if successful
// 			0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, void *dataInPtr){
    NODE *newNode = (NODE *)malloc(sizeof(NODE));
    if (newNode == NULL) return 0;

    newNode->llink = NULL;
    newNode->rlink = NULL;
    newNode->dataPtr = dataInPtr;
    

    if (pList->count == 0) {
        pList->head = newNode;
        pList->rear = newNode;
    } 
	else if (pPre == NULL) {
        newNode->rlink = pList->head;
        pList->head->llink = newNode;
        pList->head = newNode;
    } 
	else if (pPre == pList->rear) {
        pPre->rlink = newNode;
        newNode->llink = pPre;
        pList->rear = newNode;
    } 
	else {
        pPre->rlink->llink = newNode;
        newNode->rlink = pPre->rlink;
        pPre->rlink = newNode;
        newNode->llink = pPre;
    }
	pList->count++;
    return 1;
}

// internal delete function
// deletes data from list and saves the (deleted) data to dataOutPtr
// for removeNode function
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, void **dataOutPtr){
	{
    *dataOutPtr = pLoc->dataPtr; 
    
    if(pPre == NULL){
        if(pList->rear == pList->head){ // list 안에 노드가 하나일 때
            pList->head = NULL;
            pList->rear = NULL;
        }
        else{ // 노드가 한개는 아닌데 맨 앞 데이터 삭제할 때 
            pLoc->rlink->llink = NULL;
            pList->head = pLoc->rlink;
        }
    }
	
	else{
        if(pLoc == pList->rear){ // 맨 끝 데이터
            pPre->rlink = NULL; 
            pList->rear = pPre;
        }
        else{ // 중간 데이터
            pPre->rlink = pLoc->rlink;
            pLoc->rlink->llink = pPre;
        }
    }
    free(pLoc);
   
    pList->count--;
}
}

// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// for addNode, removeNode, searchNode functions
// return	1 found
// 			0 not found

static int _search( LIST *pList, NODE **pPre, NODE **pLoc, void *pArgu) {
     *pPre = NULL;
     *pLoc = pList->head;
     
     while((*pLoc != NULL) && (pList->compare((void*)pArgu, (*pLoc)->dataPtr)>0))
     {
         *pPre = *pLoc;
         *pLoc = (*pLoc)->rlink;
    }
     
     if  (*pLoc == NULL)   return 0; 
     
     else
     {
         if (pList->compare((void*)pArgu, (*pLoc)->dataPtr) == 0)      return 1; 
         else	return 0;
     }
 }

// Allocates dynamic memory for a list head node and returns its address to caller
// return	head node pointer
// 			NULL if overflow
LIST *createList( int (*compare)(const void *, const void *)){
    LIST* newList = (LIST*)malloc(sizeof(LIST));
	if(newList == NULL)	return NULL;

	newList->count = 0;
	newList->head = NULL;
	newList->rear = NULL;
    newList->compare = compare;
	return newList;
}

//  이름 리스트에 할당된 메모리를 해제 (head node, data node, name data)
void destroyList( LIST *pList, void (*callback)(void *)){
    NODE* delNode, *pNext;
    delNode = pList->head;
    
    while(delNode != NULL){
        pNext = delNode->rlink;
		(*callback)(delNode->dataPtr); 
        free(delNode);
        delNode = pNext;
    }
    pList->count = 0;
	pList->head = NULL;
    pList->rear= NULL;
    free(pList);
}

// Inserts data into list
//	return	0 if overflow
//			1 if successful
//			2 if duplicated key
int addNode( LIST *pList, void *dataInPtr){

	NODE* pPre = NULL, *pLoc = pList->head;
	int result_1, result_2;

	result_1 = _search(pList, &pPre, &pLoc, dataInPtr);
	
	if(result_1 == 0){

		result_2 = _insert(pList, pPre, dataInPtr);
        if(result_2 == 1){
            return 1;
        }
        else{
            return 0;
        }

	}
	else{
		return 2;
	}
	return 0;
}

// Removes data from list
//	return	0 not found
//			1 deleted
int removeNode( LIST *pList, void *keyPtr, void **dataOutPtr){
	
	NODE *pPre;
    NODE *pLoc = pList->head;
    
    if(_search(pList, &pPre, &pLoc, keyPtr))
    {
        _delete(pList, pPre, pLoc, dataOutPtr);
        return 1;
    }
    else return 0;
}

// interface to search function
//	pArgu	key being sought
//	dataOutPtr	contains found data
//	return	1 successful
//			0 not found
int searchNode( LIST *pList, void *pArgu, void **dataOutPtr){
	NODE *pPre = NULL;
    NODE *pLoc = pList->head;
    
	_search(pList, &pPre, &pLoc, pArgu);
	if(pPre == NULL) return 0;

	else{
        if(pList->compare((void*)pArgu, pLoc->dataPtr) == 0){
            *dataOutPtr = pLoc->dataPtr;
            return 1;
        }
        else return 0;
    }
}

// returns number of nodes in list
int countList( LIST *pList){
	return pList->count;
}

// returns	1 empty
//			0 list has data
int emptyList( LIST *pList){
	if(pList->count == 0)	return 1;
	else return 0;
}

// traverses data from list (forward)
void traverseList( LIST *pList, void (*callback)(const void *)){
	NODE *temp = pList->head;
    
    int i;
    for(i = 0 ; i < pList->count ; i++)
    {
        (*callback)(temp->dataPtr);
        temp = temp->rlink;
    }
}

// traverses data from list (backward)
void traverseListR( LIST *pList, void (*callback)(const void *)){
	NODE *temp = pList->rear;
    
    int i;
    for(i = 0; i < pList->count ; i++)
    {
        (*callback)(temp->dataPtr);
        temp = temp->llink;
    }
}
