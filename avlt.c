#define BALANCING

#include <stdlib.h> // malloc
#include <stdio.h>

#include "avlt.h"

#define max(x, y) (((x) > (y)) ? (x) : (y))
static NODE *rotateRight(NODE *root);
static NODE *rotateLeft(NODE *root);
static int getHeight(NODE *root);
// internal functions (not mandatory)
// used in AVLT_Insert
// return 	pointer to root // AVL_Insert 안에서 rebalancing 과정까지 해야함
static NODE *_insert(NODE *root, NODE *newPtr, int (*compare)(const void *, const void *))
{
    if (root == NULL)
        return newPtr;

    int cmp_result = compare(newPtr->dataPtr, root->dataPtr);
    int b_factor;

    if (cmp_result < 0)
    {
        root->left = _insert(root->left, newPtr, compare); // root->left에 다른 노드가 있음
        root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
        // recursion으로.. _insert의 return 값이 이전의/새로운 값이므로 이전의 자리에 삽입
        //  기존 subtree root의 왼쪽 값이 변할 수 있다면 그 변한 left subtree, right subtree 높이 비교
        b_factor = getHeight(root->left) - getHeight(root->right);

        if (b_factor > 1)
        {
            // left high, unbalanced
            if (getHeight(root->left->left) > getHeight(root->left->right))
            {
                // left of left high
                root->height--; // 왜 줄이는지 다시 확인
                root = rotateRight(root);
                root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
            }
            else if (getHeight(root->left->left) < getHeight(root->left->right))
            {
                // right of left high
                root->height--;
                root->left = rotateLeft(root->left);
                root->left->height = max(getHeight(root->left->left), getHeight(root->left->right)) + 1;
                root = rotateRight(root);
                root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
            }
        }
    }
    else if (cmp_result > 0)
    { // root보다 큼
        root->right = _insert(root->right, newPtr, compare);
        root->height = max(getHeight(root->left), getHeight(root->right)) + 1;

        b_factor = getHeight(root->left) - getHeight(root->right);

        if (b_factor < -1)
        {
            // right high, unbalanced
            if (getHeight(root->right->left) > getHeight(root->right->right))
            {
                // left of right high
                root->height--;
                root->right = rotateRight(root->right);
                root->right->height = max(getHeight(root->right->left), getHeight(root->right->right)) + 1;
                root = rotateLeft(root);
                root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
            }
            else if (getHeight(root->right->left) < getHeight(root->right->right))
            {
                // right of right high
                root->height--;
                root = rotateLeft(root);
                root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
            }
        }
    }
    // 삽입 과정을 마쳤다면
    return root; // inserted node의 root, 즉, 삽입한 노드의 바로 한세대 부모의 주소
    // 만약 root 가 그대로이거나.. 바뀌었따면 이걸 루트로 삼겠다는 것
}

// used in AVLT_Insert
static NODE *_makeNode(void *dataInPtr)
{
    NODE *newNode = (NODE *)malloc(sizeof(NODE));
    if (newNode == NULL)
        return NULL;

    newNode->dataPtr = dataInPtr;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->height = 1; //? 업데이트 시  height 처리 방법

    return newNode;
}

// used in AVLT_Destroy
static void _destroy(NODE *root, void (*callback)(void *))
{
    NODE *temp;
    if (root != NULL)
    {
        temp = root->right;
        _destroy(root->left, callback);
        callback(root->dataPtr);
        free(root);
        _destroy(temp, callback);
    }
}

// used in AVLT_Delete
// return 	pointer to root
static NODE *_delete(NODE *root, void *keyPtr, void **dataOutPtr, int (*compare)(const void *, const void *))
{
    NODE *parent, *out, *find, *trace;
    parent = NULL;
    int cmp_result = compare(keyPtr, root->dataPtr);
    while ((root != NULL) && (cmp_result != 0))
    {
        if (cmp_result < 0)
        {
            parent = root;
            root = root->left;
        }
        else
        {
            parent = root;
            root = root->right;
        }

        cmp_result = compare(keyPtr, root->dataPtr);
    }

    if (root == NULL)
    {
        *dataOutPtr = NULL;
        return NULL;
    }

    *dataOutPtr = root->dataPtr;

    if (root->left == NULL)
    {
        if (root->right == NULL)
        {
            // no child
            if (parent != NULL)
            {
                if (parent->left == root)
                    parent->left = NULL;
                else if (parent->right == root)
                    parent->right = NULL;
            }

            free(root);
            return NULL;
        }
        else
        {
            // only have right child
            if (parent != NULL)
            {
                if (parent->left == root)
                    parent->left = root->right;
                else if (parent->right == root)
                    parent->right = root->right;
            }
            out = root->right;

            free(root);
            return out;
        }
    }
    else
    {
        // left isn't NULL
        if (root->right == NULL)
        {
            // only left child
            if (parent != NULL)
            {
                if (parent->left == root)
                    parent->left = root->left;
                else if (parent->right == root)
                    parent->right = root->left;
            }
            out = root->left;

            free(root);
            return out;
        }
        else
        {
            // have two children
            find = root->right;
            trace = root; // parent of find
            while (find->left != NULL)
            {
                trace = find; // parent of find
                find = find->left;
            }

            if (find == trace->right)
            {
                trace->right = find->right;
                find->left = root->left;
                find->right = root->right;
            }
            else // find == trace->left
            {
                trace->left = find->right; // could be NULL or subtree
                find->left = root->left;
                find->right = root->right;
            }

            if (parent != NULL)
            {
                if (parent->left == root)
                    parent->left = find;
                else if (parent->right == root)
                    parent->right = find;
            }
            free(root);
            return find;
        }
    }
}

// used in AVLT_Search
// Retrieve node containing the requested key
// return	address of the node containing the key
//			NULL not found
static NODE *_search(NODE *root, void *keyPtr, int (*compare)(const void *, const void *))
{
    NODE *Root = root;
    int cmp_result;

    while (1)
    {
        cmp_result = compare(keyPtr, Root->dataPtr);

        if (cmp_result == 0)
        {
            return Root;
        }
        else if (cmp_result < 0)
        {
            Root = Root->left;
            if (Root == NULL)
                return NULL;
        }
        else
        {
            Root = Root->right;
            if (Root == NULL)
                return NULL;
        }
    }
}

// used in AVLT_Traverse
static void _traverse(NODE *root, void (*callback)(const void *))
{
    if (root != NULL)
    {
        _traverse(root->left, callback);
        callback(root->dataPtr);
        _traverse(root->right, callback);
    }
}

// used in AVLT_TraverseR
static void _traverseR(NODE *root, void (*callback)(const void *))
{
    if (root != NULL)
    {
        _traverseR(root->right, callback);
        callback(root->dataPtr);
        _traverseR(root->left, callback);
    }
}

// used in printTree
static void _inorder_print(NODE *root, int level, void (*callback)(const void *))
{
    int i;
    if (root != NULL)
    {
        _inorder_print(root->right, level + 1, callback);
        for (i = 0; i < level; i++)
        {
            printf("\t");
        }
        callback(root->dataPtr);
        _inorder_print(root->left, level + 1, callback);
    }
}

// internal function
// return	height of the (sub)tree from the node (root)
static int getHeight(NODE *root)
{
    if (root == NULL)
        return 0;
    else
        return root->height;
}

// internal function
// Exchanges pointers to rotate the tree to the right
// updates heights of the nodes
// return	new root
static NODE *rotateRight(NODE *root)
{
    NODE *temp, *root_new;
    temp = root->left->right;
    root_new = root->left;
    root_new->right = root;
    root->left = temp;

    root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
    return root_new;
}

// internal function
// Exchanges pointers to rotate the tree to the left
// updates heights of the nodes
// return	new root
static NODE *rotateLeft(NODE *root)
{
    NODE *temp, *root_new;
    temp = root->right->left;
    root_new = root->right;
    root_new->left = root;
    root->right = temp;

    root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
    return root_new;
}

/* Allocates dynamic memory for a tree head node and returns its address to caller
    return	head node pointer
            NULL if overflow
*/
TREE *AVLT_Create(int (*compare)(const void *, const void *))
{
    TREE *newT = (TREE *)malloc(sizeof(TREE));
    if (newT == NULL)
        return NULL;

    newT->count = 0;
    newT->compare = compare;
    newT->root = NULL;

    return newT;
}

/* Deletes all data in tree and recycles memory
 */
void AVLT_Destroy(TREE *pTree, void (*callback)(void *))
{
    _destroy(pTree->root, callback);
    free(pTree);
}

/* Inserts new data into the tree
    return	1 success
            0 overflow
*/
int AVLT_Insert(TREE *pTree, void *dataInPtr)
{
    NODE *newNode = _makeNode(dataInPtr);
    if (newNode == NULL)
        return 0;

    if (pTree->count == 0)
    {
        pTree->root = newNode;
    }
    else
    {
        pTree->root = _insert(pTree->root, newNode, pTree->compare);
    }
    pTree->count++;

    // insert 한 후.. balance 맞추는 과정
    // root_of_insert 에 대해

    return 1;
}

/* Deletes a node with keyPtr from the tree
    return	address of data of the node containing the key
            NULL not found
*/
void *AVLT_Delete(TREE *pTree, void *keyPtr)
{
    NODE *temp;
    void *dataOutPtr;
    NODE *tsearch = _search(pTree->root, keyPtr, pTree->compare);
    if (tsearch == NULL)
        return NULL;

    if (pTree->root == tsearch) // 지우려는 값이 root 일 때
    {

        temp = _delete(pTree->root, keyPtr, &dataOutPtr, pTree->compare);
        if (temp == NULL)
            pTree->root = NULL;
        else
            pTree->root = temp;
    }
    else
    {
        // 지우려는게 root가 아님
        temp = _delete(pTree->root, keyPtr, &dataOutPtr, pTree->compare);
        if (temp == NULL)
        {
            pTree->count--;
            return (void *)dataOutPtr;
        }
        else
        {
            tsearch = temp;
        }
    }
    pTree->count--;
    return (void *)dataOutPtr;
}

/* Retrieve tree for the node containing the requested key (keyPtr)
    return	address of data of the node containing the key
            NULL not found
*/
void *AVLT_Search(TREE *pTree, void *keyPtr)
{
    NODE *result = _search(pTree->root, keyPtr, pTree->compare);
    if (result == NULL)
        return NULL;

    return result->dataPtr;
}

/* prints tree using inorder traversal
 */
void AVLT_Traverse(TREE *pTree, void (*callback)(const void *))
{
    _traverse(pTree->root, callback);
}

/* prints tree using right-to-left inorder traversal
 */
void AVLT_TraverseR(TREE *pTree, void (*callback)(const void *))
{
    _traverseR(pTree->root, callback);
}

/* Print tree using right-to-left inorder traversal with level
 */
void printTree(TREE *pTree, void (*callback)(const void *))
{
    _inorder_print(pTree->root, 0, callback);
}

/* returns number of nodes in tree
 */
int AVLT_Count(TREE *pTree)
{
    return pTree->count;
}

/* returns height of the tree
 */
int AVLT_Height(TREE *pTree)
{
    if (pTree->count == 0)
        return -1;
    return pTree->root->height;
}
