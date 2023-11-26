#include <stdlib.h> // malloc
#include <stdio.h>

#include "bst.h"

// internal functions (not mandatory)
// used in BST_Insert
static void _insert(NODE *root, NODE *newPtr, int (*compare)(const void *, const void *))
{

    int cmp_result = compare(newPtr->dataPtr, root->dataPtr);

    if (cmp_result < 0)
    {
        if (root->left == NULL)
            root->left = newPtr;
        else
            _insert(root->left, newPtr, compare); // root->left에 다른 노드가 있음
    }
    else
    { // root보다 크거나 같음
        if (root->right == NULL)
            root->right = newPtr;
        else
            _insert(root->right, newPtr, compare);
    }
}

// used in BST_Insert
static NODE *_makeNode(void *dataInPtr)
{
    NODE *newNode = (NODE *)malloc(sizeof(NODE));
    if (newNode == NULL)
        return NULL;

    newNode->dataPtr = dataInPtr;
    newNode->left = NULL;
    newNode->right = NULL;

    return newNode;
}

// used in BST_Destroy
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

// used in BST_Delete
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

// used in BST_Search
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

// used in BST_Traverse
static void _traverse(NODE *root, void (*callback)(const void *))
{
    if (root != NULL)
    {
        _traverse(root->left, callback);
        callback(root->dataPtr);
        _traverse(root->right, callback);
    }
}

// used in BST_TraverseR
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

/* Allocates dynamic memory for a tree head node and returns its address to caller
    return	head node pointer
            NULL if overflow
*/
TREE *BST_Create(int (*compare)(const void *, const void *))
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
void BST_Destroy(TREE *pTree, void (*callback)(void *))
{
    _destroy(pTree->root, callback);
    free(pTree);
}

/* Inserts new data into the tree
    return	1 success
            0 overflow
*/
int BST_Insert(TREE *pTree, void *dataInPtr)
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
        _insert(pTree->root, newNode, pTree->compare);
    }
    pTree->count++;
    return 1;
}

/* Deletes a node with keyPtr from the tree
    return	address of data of the node containing the key
            NULL not found
*/
void *BST_Delete(TREE *pTree, void *keyPtr)
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
void *BST_Search(TREE *pTree, void *keyPtr)
{
    NODE *result = _search(pTree->root, keyPtr, pTree->compare);
    if (result == NULL)
        return NULL;

    return result->dataPtr;
}

/* prints tree using inorder traversal
 */
void BST_Traverse(TREE *pTree, void (*callback)(const void *))
{
    _traverse(pTree->root, callback);
}

/* prints tree using right-to-left inorder traversal
 */
void BST_TraverseR(TREE *pTree, void (*callback)(const void *))
{
    _traverseR(pTree->root, callback);
}

/* Print tree using right-to-left inorder traversal with level
 */
void printTree(TREE *pTree, void (*callback)(const void *))
{
    _inorder_print(pTree->root, 0, callback);
}

/*
    returns number of nodes in tree
*/
int BST_Count(TREE *pTree)
{
    return pTree->count;
}
// 문제되는거 지금 delete 밖에 없긴한데.. 지우고 나서 지워진 자리에 대체할 값이 그 자리에 안 들어와서 그런듯