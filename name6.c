#include <stdio.h>
#include <stdlib.h> // malloc
#include <string.h> // strdup, strcmp
#include <ctype.h>	// toupper

#include "bst.h"

#define QUIT 1
#define FORWARD_PRINT 2
#define BACKWARD_PRINT 3
#define TREE_PRINT 4
#define SEARCH 5
#define DELETE 6
#define COUNT 7

// User structure type definition
typedef struct
{
	char *name; // 이름
	char sex;	// 성별 M or F
	int freq;	// 빈도
} tName;

////////////////////////////////////////////////////////////////////////////////
// Allocates dynamic memory for a name structure, initialize fields(name, sex, freq) and returns its address to caller
//	return	name structure pointer
//			NULL if overflow
tName *createName(char *name, char sex, int freq);

// Deletes all data in name structure and recycles memory
void destroyName(void *pName);

////////////////////////////////////////////////////////////////////////////////
// gets user's input
int get_action()
{
	char ch;
	scanf("%c", &ch);
	ch = toupper(ch);
	switch (ch)
	{
	case 'Q':
		return QUIT;
	case 'P':
		return FORWARD_PRINT;
	case 'B':
		return BACKWARD_PRINT;
	case 'T':
		return TREE_PRINT;
	case 'S':
		return SEARCH;
	case 'D':
		return DELETE;
	case 'C':
		return COUNT;
	}
	return 0; // undefined action
}

// compares two names in name structures
// for BST_Create functions
int cmpName(const void *pName1, const void *pName2)
{
	int ret = strcmp(((tName *)pName1)->name, ((tName *)pName2)->name);
	if (ret != 0)
		return ret;

	return ((tName *)pName1)->sex - ((tName *)pName2)->sex;
}

// prints contents of name structure
// for traverseList and traverseListR functions
void print_name(const void *dataPtr)
{
	printf("%s\t%c\t%d\n", ((tName *)dataPtr)->name, ((tName *)dataPtr)->sex, ((tName *)dataPtr)->freq);
}

// prints name in name structure
// for printTree function
void print_name_only(const void *dataPtr)
{
	printf("%s\n", ((tName *)dataPtr)->name);
}

// gets user's input (name/sex)
void input_name(char *name, char *sex)
{
	char *p;

	while (1)
	{
		fprintf(stderr, "Input a name/sex to find: ");
		fscanf(stdin, "%s", name);

		if ((p = strchr(name, '/')) == NULL)
		{
			fprintf(stderr, "invalid name/sex!\n");
			continue;
		}
		*p++ = 0;
		if (*p && (*p == 'M' || *p == 'F'))
			*sex = *p;
		else
		{
			fprintf(stderr, "invalid sex! (M | F)\n");
			continue;
		}
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	TREE *tree;

	char name[100];
	char sex;
	int freq;

	tName *pName;
	int ret;
	FILE *fp;

	if (argc != 2)
	{
		fprintf(stderr, "usage: %s FILE\n", argv[0]);
		return 1;
	}

	fp = fopen(argv[1], "rt");
	if (!fp)
	{
		fprintf(stderr, "Error: cannot open file [%s]\n", argv[1]);
		return 2;
	}

	// creates an empty tree
	tree = BST_Create(cmpName);
	if (!tree)
	{
		printf("Cannot create a tree\n");
		return 100;
	}

	while (fscanf(fp, "%s\t%c\t%d", name, &sex, &freq) != EOF)
	{
		pName = createName(name, sex, freq);

		ret = BST_Insert(tree, pName);

		if (ret == 0) // failure
		{
			destroyName(pName);
		}
	}

	fclose(fp);

	fprintf(stderr, "Select Q)uit, P)rint, B)ackward print, T)ree print, S)earch, D)elete, C)ount: ");

	while (1)
	{
		void *ptr;
		int action = get_action();

		switch (action)
		{
		case QUIT:
			BST_Destroy(tree, destroyName);
			return 0;

		case FORWARD_PRINT:
			BST_Traverse(tree, print_name);
			break;

		case BACKWARD_PRINT:
			BST_TraverseR(tree, print_name);
			break;

		case TREE_PRINT:
			printTree(tree, print_name_only);
			break;

		case SEARCH:
			input_name(name, &sex);

			pName = createName(name, sex, 0);

			if ((ptr = BST_Search(tree, pName)) != NULL)
				print_name(ptr);
			else
				fprintf(stdout, "%s/%c not found\n", name, sex);

			destroyName(pName);
			break;

		case DELETE:
			input_name(name, &sex);

			pName = createName(name, sex, 0);

			if ((ptr = BST_Delete(tree, pName)) != NULL)
			{
				fprintf(stdout, "(%s/%c, %d) deleted\n", ((tName *)ptr)->name, ((tName *)ptr)->sex, ((tName *)ptr)->freq);
				destroyName((tName *)ptr);
			}
			else
				fprintf(stdout, "%s not found\n", name);

			destroyName(pName);
			break;

		case COUNT:
			fprintf(stdout, "%d\n", BST_Count(tree));
			break;
		}

		if (action)
			fprintf(stderr, "Select Q)uit, P)rint, B)ackward print, T)ree print, S)earch, D)elete, C)ount: ");
	}
	return 0;
}

tName *createName(char *name, char sex, int freq)
{
	tName *newName = (tName *)malloc(sizeof(tName));
	if (newName == NULL)
		return NULL;
	newName->name = strdup(name);
	newName->sex = sex;
	newName->freq = freq;
	return newName;
}

void destroyName(void *pName)
{

	free(((tName *)pName)->name);
	free((tName *)pName);
}
