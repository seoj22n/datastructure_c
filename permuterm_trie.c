#include <stdio.h>
#include <stdlib.h> // malloc
#include <string.h> // strdup
#include <ctype.h>	// isupper, tolower

#define MAX_DEGREE 27 // 'a' ~ 'z' and EOW
#define EOW '$'		  // end of word

// used in the following functions: trieInsert, trieSearch, triePrefixList
#define getIndex(x) (((x) == EOW) ? MAX_DEGREE - 1 : ((x) - 'a'))

// TRIE type definition
typedef struct trieNode
{
	int index; // -1 (non-word), 0, 1, 2, ...
	struct trieNode *subtrees[MAX_DEGREE];
} TRIE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a trie node and returns its address to caller
	return	node pointer
			NULL if overflow
*/
TRIE *trieCreateNode(void)
{
	TRIE *newTrie = (TRIE *)malloc(sizeof(TRIE));
	if (newTrie == NULL)
		return NULL;
	newTrie->index = -1;
	for (int i = 0; i < MAX_DEGREE; i++)
	{
		newTrie->subtrees[i] = NULL;
	}
	return newTrie;
}

/* Deletes all data in trie and recycles memory
 */
void trieDestroy(TRIE *root)
{
	for (int i = 0; i < MAX_DEGREE; i++)
	{
		if (root->subtrees[i] != NULL)
		{
			trieDestroy(root->subtrees[i]);
		}
	}
	free(root);
}

/* Inserts new entry into the trie
	return	1 success
			0 failure
*/
// 주의! 엔트리를 중복 삽입하지 않도록 체크해야 함
// 대소문자를 소문자로 통일하여 삽입
// 영문자와 EOW 외 문자를 포함하는 문자열은 삽입하지 않음
int trieInsert(TRIE *root, char *str, int dic_index)
{
	int len = strlen(str), str_index;
	for (int i = 0; i < len; i++)
	{
		str_index = getIndex(str[i]);
		if (root->subtrees[str_index] == NULL)
		{
			root->subtrees[str_index] = trieCreateNode();
			if (root->subtrees[str_index] == NULL)
			{
				return 0;
			}
		}

		root = root->subtrees[str_index];
	}
	root->index = dic_index;
	return 1;
}

/* Retrieve trie for the requested key
	return	index in dictionary (trie) if key found
			-1 key not found
*/
int trieSearch(TRIE *root, char *str)
{
	int len, temp_index;
	char *end = "$";
	char temp[100];
	strcpy(temp, str);
	strcat(temp, end);
	len = strlen(temp);
	if (root == NULL)
		return -1;
	for (int i = 0; i < len; i++)
	{
		temp_index = getIndex(temp[i]);
		if (root->subtrees[temp_index] == NULL)
			return -1;
		root = root->subtrees[temp_index];
	}
	return root->index;
}

/* prints all entries in trie using preorder traversal
 */
void trieList(TRIE *root, char *dic[])
{
	if (root->index != -1)
	{
		printf("%s\n", dic[root->index]);
	}
	for (int i = 0; i < MAX_DEGREE; i++)
	{
		if (root->subtrees[i] != NULL)
		{
			trieList(root->subtrees[i], dic);
		}
	}
}

/* prints all entries starting with str (as prefix) in trie
	ex) "ab" -> "abandoned", "abandoning", "abandonment", "abased", ...
	this function uses trieList function
*/
void triePrefixList(TRIE *root, char *str, char *dic[])
{
	int index;
	while (*str)
	{
		index = getIndex(*str);
		if (root->subtrees[index])
			root = root->subtrees[index];
		str++;
	}
	trieList(root, dic);
}

/* makes permuterms for given str
	ex) "abc" -> "abc$", "bc$a", "c$ab", "$abc"
	return	number of permuterms
*/

int make_permuterms(char *str, char *permuterms[])
{
	int len = strlen(str);
	char *str_permute = (char *)malloc((len + 2) * sizeof(char));
	if (str_permute == NULL)
	{
		// Handle memory allocation failure
		return 0;
	}

	char *eow = "$";

	strcpy(str_permute, str);
	strcat(str_permute, eow);

	permuterms[0] = strdup(str_permute);

	for (int i = 0; i < len; i++)
	{
		char temp = str_permute[0];
		for (int j = 0; j < len; j++)
		{
			str_permute[j] = str_permute[j + 1];
		}
		str_permute[len - 1] = temp;
		permuterms[i + 1] = strdup(str_permute);
	}

	free(str_permute);

	return len + 1;
}

/* recycles memory for permuterms
 */
void clear_permuterms(char *permuterms[], int size)
{
	for (int i = 0; i < size; i++)
	{
		free(permuterms[i]);
	}
}

/* wildcard search
	ex) "ab*", "*ab", "a*b", "*ab*"
	this function uses triePrefixList function
*/
void trieSearchWildcard(TRIE *root, char *str, char *dic[])
{
	char *eow = "$", *wild = "*";
	int len = strlen(str);

	// Allocate dynamic memory for the 'word' array
	char *word = (char *)malloc((len + 2) * sizeof(char));
	if (word == NULL)
	{
		// Handle memory allocation failure
		return;
	}

	if (str[0] == 42)
	{
		if (str[len - 1] == 42)
		{
			for (int i = 0; i < len - 2; i++)
				word[i] = str[i + 1];
			word[len - 2] = '\0'; // Null-terminate the word
			triePrefixList(root, word, dic);
		}
		else
		{
			for (int i = 0; i < len - 1; i++)
				word[i] = str[i + 1];
			word[len - 1] = '\0'; // Null-terminate the word
			strcat(word, eow);
			triePrefixList(root, word, dic);
		}
	}
	else if (str[len - 1] == 42)
	{
		word[0] = '\0'; // Start with an empty string
		strcat(word, eow);
		for (int i = 0; i < len - 1; i++)
			word[i + 1] = str[i];
		word[len] = '\0'; // Null-terminate the word
		triePrefixList(root, word, dic);
	}
	else
	{
		char *token1 = strtok(str, wild);
		char *token2 = strtok(NULL, " ");
		strcpy(word, token2);
		strcat(word, eow);
		strcat(word, token1);
		triePrefixList(root, word, dic);
	}

	// Free the dynamically allocated memory
	free(word);
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	TRIE *permute_trie;
	char *dic[100000];

	int ret;
	char str[100];
	FILE *fp;
	char *permuterms[100];
	int num_p; // # of permuterms
	int word_index = 0;

	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s FILE\n", argv[0]);
		return 1;
	}

	fp = fopen(argv[1], "rt");
	if (fp == NULL)
	{
		fprintf(stderr, "File open error: %s\n", argv[1]);
		return 1;
	}

	permute_trie = trieCreateNode(); // trie for permuterm index

	while (fscanf(fp, "%s", str) != EOF)
	{
		num_p = make_permuterms(str, permuterms);

		for (int i = 0; i < num_p; i++)
			trieInsert(permute_trie, permuterms[i], word_index);

		clear_permuterms(permuterms, num_p);

		dic[word_index++] = strdup(str);
	}

	fclose(fp);

	printf("\nQuery: ");
	while (fscanf(stdin, "%s", str) != EOF)
	{
		// wildcard search term
		if (strchr(str, '*'))
		{
			trieSearchWildcard(permute_trie, str, dic);
		}
		// keyword search
		else
		{
			ret = trieSearch(permute_trie, str);

			if (ret == -1)
				printf("[%s] not found!\n", str);
			else
				printf("[%s] found!\n", dic[ret]);
		}
		printf("\nQuery: ");
	}

	for (int i = 0; i < word_index; i++)
		free(dic[i]);

	trieDestroy(permute_trie);

	return 0;
}
