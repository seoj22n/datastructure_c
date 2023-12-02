#include <stdio.h>
#include <stdlib.h> // malloc, realloc, free

#include "adt_heap.h"

/* Allocates memory for heap and returns address of heap head structure
if memory overflow, NULL returned
The initial capacity of the heap should be 10
*/
static void _reheapDown(HEAP *heap, int index);
static void _reheapUp(HEAP *heap, int index);

HEAP *heap_Create(int (*compare)(const void *arg1, const void *arg2))
{
    HEAP *newHeap = (HEAP *)malloc(sizeof(HEAP));
    if (newHeap == NULL)
        return NULL;

    newHeap->capacity = 10;
    newHeap->compare = compare;
    newHeap->heapArr = (void **)malloc(sizeof(void *) * newHeap->capacity);
    if (newHeap->heapArr == NULL)
        return NULL;

    for (int i = 0; i < newHeap->capacity; i++)
    {
        newHeap->heapArr[i] = NULL;
    }
    newHeap->last = -1;
    return newHeap;
}

/* Free memory for heap
 */
void heap_Destroy(HEAP *heap, void (*remove_data)(void *ptr))
{
    for (int i = 0; i < heap->last + 1; i++)
    {
        remove_data(heap->heapArr[i]);
    }

    free(heap->heapArr);
    free(heap);
    return;
}

/* Inserts data into heap
return 1 if successful; 0 if heap full
*/
int heap_Insert(HEAP *heap, void *dataPtr)
{
    if (heap->last == -1)
    {
        *((heap->heapArr) + 0) = dataPtr;
        heap->last = 0;
    }
    else
    {
        if (heap->capacity == heap->last + 1) // overflow case 에 공간 확장
        {
            int capacity = heap->capacity;
            heap->heapArr = (void **)realloc(heap->heapArr, sizeof(void *) * (capacity + 100));
            if (heap->heapArr == NULL)
                return 0;

            for (int i = capacity; i < capacity + 100; i++)
            {
                heap->heapArr[i] = NULL;
            }

            heap->capacity += 100;
        }

        *((heap->heapArr) + heap->last + 1) = dataPtr;
        heap->last += 1;
    }
    // 삽입은 마쳤고 이제 reheapup으로 정렬
    _reheapUp(heap, heap->last);

    return 1;
}

/* Deletes root of heap and passes data back to caller
return 1 if successful; 0 if heap empty
*/
int heap_Delete(HEAP *heap, void **dataOutPtr)
{
    if (heap->last == -1)
        return 0;

    void *temp;
    *dataOutPtr = *(heap->heapArr + 0);

    if (heap->last > 0)
    {
        temp = *(heap->heapArr + 0);
        *(heap->heapArr + 0) = *(heap->heapArr + heap->last);
        *(heap->heapArr + heap->last) = temp;

        heap->last--;

        _reheapDown(heap, 0);
    }
    else
    { // heap->last == 0
        heap->last = -1;
    }
    return 1;
}

/*
return 1 if the heap is empty; 0 if not
*/
int heap_Empty(HEAP *heap)
{
    if (heap->last < 0)
        return 1;
    else
        return 0;
}

/* Print heap array */
void heap_Print(HEAP *heap, void (*print_func)(const void *data))
{
    int i = 0;
    while (heap->last >= i)
    {
        print_func(*(heap->heapArr + i));
        i++;
    }
    return;
}

/* Reestablishes heap by moving data in child up to correct location heap array
 */
static void _reheapUp(HEAP *heap, int index)
{
    if (index == 0)
        return;

    int parent_index = (index - 1) / 2;
    void *temp;
    int cmp_result = heap->compare(*(heap->heapArr + index), *(heap->heapArr + parent_index));

    if (cmp_result > 0)
    {
        temp = *(heap->heapArr + parent_index);
        *(heap->heapArr + parent_index) = *(heap->heapArr + index);
        *(heap->heapArr + index) = temp;
        _reheapUp(heap, parent_index);
    }

    return;
}

/* Reestablishes heap by moving data in root down to its correct location in the heap
 */
static void _reheapDown(HEAP *heap, int index)
{
    int left, right, bigger_index, cmp_result;
    void *temp;

    left = index * 2 + 1;
    right = index * 2 + 2;
    if (left <= heap->last)
    {
        if (right <= heap->last)
        {
            // left, right 둘 다 있음
            cmp_result = heap->compare(*(heap->heapArr + left), *(heap->heapArr + right));
            if (cmp_result >= 0)
                bigger_index = left;
            else
                bigger_index = right;

            cmp_result = heap->compare(*(heap->heapArr + index), *(heap->heapArr + bigger_index));
            if (cmp_result < 0)
            {
                temp = *(heap->heapArr + index);
                *(heap->heapArr + index) = *(heap->heapArr + bigger_index);
                *(heap->heapArr + bigger_index) = temp;
            }
            _reheapDown(heap, bigger_index);
        }
        else
        {
            // left만 있고 right는 없음
            cmp_result = heap->compare(*(heap->heapArr + index), *(heap->heapArr + left));
            if (cmp_result < 0)
            {
                temp = *(heap->heapArr + index);
                *(heap->heapArr + index) = *(heap->heapArr + left);
                *(heap->heapArr + left) = temp;
                _reheapDown(heap, left);
            }
        }
    }
    else
    {
        // 둘 다 없음. 끝 즉, leaf에 위치
        return;
    }
}