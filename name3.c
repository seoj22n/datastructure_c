#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_YEAR_DURATION	10	// 기간
#define SORT_BY_NAME	0 // 이름 순 정렬
#define SORT_BY_FREQ	1 // 빈도 순 정렬

// 이름 구조체 선언
typedef struct {
	char	name[20];		// 이름
	char	sex;			// 성별 M or F
	int		total_freq;		// 연도별 빈도 합
	int		freq[MAX_YEAR_DURATION]; // 연도별 빈도
} tName;

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	tName		*dataPtr;
	struct node	*link; // 이름순 리스트를 위한 포인터
	struct node	*link2; // 빈도순 리스트를 위한 포인터
} NODE;

typedef struct
{
	int		count;
	NODE	*head; // 이름순 리스트의 첫번째 노드에 대한 포인터
	NODE	*head2; // 빈도순 리스트의 첫번째 노드에 대한 포인터
} LIST;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations


// Allocates dynamic memory for a list head node and returns its address to caller
// return	head node pointer
// 			NULL if overflow
LIST *createList(void){
    LIST* newList = (LIST*)malloc(sizeof(LIST));
    if(newList == NULL) return NULL;
    
    newList->count = 0;
    newList->head = NULL;
    newList->head2 = NULL;
    return newList;
}

//  이름 리스트에 할당된 메모리를 해제 (head node, data node, name data)
void destroyList( LIST *pList){
    NODE* delNode, *pNext;
    delNode = pList->head;
    
    while(delNode != NULL){
        pNext = delNode->link;

        free(delNode->dataPtr);
        free(delNode);
        delNode = pNext;
    }
    pList->count = 0;
    free(pList);
}


// internal insert function
// inserts data into a new node
// return	1 if successful
// 			0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, tName *dataInPtr){
    NODE* newNode = (NODE*)malloc(sizeof(NODE));
    if(newNode == NULL){
    free(newNode);
    return 0;
    }
   
    newNode->dataPtr = dataInPtr;
    newNode->link = NULL;
    newNode->link2 = NULL; //노드 생성 후  초화
	pList->count ++;
    

    if(pPre==NULL){ // 맨 앞에 삽입 또는 빈  리트 삽ㅣㅂ
        newNode->link = pList->head;
        pList->head = newNode;
    }
    else{ 
        newNode->link = pPre->link;
        pPre->link = newNode;
    }
    return 1;
}

// internal function
// connects node into a frequency list
static void _link_by_freq(LIST *pList, NODE *pPre, NODE *pLoc) {
    
    if (pPre == NULL) {
        pLoc->link2 = pList->head2;
        pList->head2 = pLoc;
    } else {
        pLoc->link2 = pPre->link2;
        pPre->link2 = pLoc;
    }
}

// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// return	1 found
// 			0 not found
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, tName *pArgu){

	*pPre = NULL;
	*pLoc = pList->head;

        
	while(((*pLoc) != NULL) && (strcmp(pArgu->name, (*pLoc)->dataPtr->name)>=0)){ 
		if(strcmp(pArgu->name, (*pLoc)->dataPtr->name)==0){
			
			if(pArgu->sex == (*pLoc)->dataPtr->sex)	return 1; // 이름이 같고 성별까지 같으면..
            else if(pArgu->sex < (*pLoc)->dataPtr->sex) return 0; // 만약 argu->sex 가 F라면 원래 있던게 자동 M 이므로 업데이트없이 그자리에 삽입해야댐
            else{ // 만약 argu -> sex 가 M 이라면, 즉, 원래 있던게 F라서 먼저 나와있는거라면 한 칸 더 이동해서 M 있는지 확인해봐야댐
                    (*pPre) = (*pLoc);
                    (*pLoc) = (*pLoc)->link; // 한 칸 더 이동해보고
                    
                        if((strcmp(pArgu->name, (*pLoc)->dataPtr->name) == 0) && pArgu->sex == (*pLoc)->dataPtr->sex) 
                            return 1; // 근데 아예 다른 데이터고 우연히 성별만 같을수도 있으니 이름이랑 성별 둘다체크

                        else return 0; // 여기서도 없으면 진짜 없는거
            }
		}
		
			(*pPre) = (*pLoc);
		    (*pLoc) = (*pLoc)->link;
	}
	return 0;
}

// Internal search function for frequency-sorted list
// Searches list and passes back address of the node containing target and its logical predecessor
// Returns 1 if found, 0 if not found


static int _search_by_freq(LIST *pList, NODE **pPre, NODE **pLoc, tName *pArgu) {
    *pPre = NULL;
    *pLoc = pList->head2; // The current node for comparison, linked by link2.
    int name_compare;

    while (*pLoc != NULL && pArgu->total_freq <= (*pLoc)->dataPtr->total_freq) {
        if (pArgu->total_freq == (*pLoc)->dataPtr->total_freq) {

            name_compare = strcmp(pArgu->name, (*pLoc)->dataPtr->name);

            if(name_compare == 0){
                if (pArgu->sex < (*pLoc)->dataPtr->sex) {
                    return 0; 
                } 
                
                else {
                    *pPre = *pLoc;
                    *pLoc = (*pLoc)->link2;
                    return 0;
                }
            }

            else if(name_compare<0){
                return 0;
            }
            // 이름이 더 크면 그냥 진행시키면됨.. 왜냐면 뒤에 총빈도 같고 이름만 다른게 또 있을 수도 있으니까
        }
        
        *pPre = *pLoc;
        *pLoc = (*pLoc)->link2;
    }
    return 0; // Not found.
}



// 이름 구조체를 위한 메모리를 할당하고, 이름(name)과 성별(sex)을 초기화
// return	할당된 이름 구조체에 대한 pointer
//			NULL if overflow
tName *createName( char *name, char sex){
    tName* newName = (tName*)malloc(sizeof(tName));
    if(newName == NULL) return NULL;

    strcpy(newName->name, name);
    newName->sex = sex;
    memset(newName->freq, 0, sizeof(newName->freq));
    newName->total_freq = 0;

    return newName;
}

//  이름 구조체에 할당된 메모리를 해제
void destroyName( tName *pNode){
    free(pNode);
}

////////////////////////////////////////////////////////////////////////////////
// 입력 파일을 읽어 이름 정보(연도, 이름, 성별, 빈도)를 이름 리스트에 저장
// 이미 리스트에 존재하는(저장된) 이름은 해당 연도의 빈도만 저장s
// 새로 등장한 이름은 리스트에 추가
// 주의사항: 동일 이름이 남/여 각각 사용될 수 있으므로, 이름과 성별을 구별해야 함
// 주의사항: 정렬 리스트(ordered list)를 유지해야 함
// start_year : 시작 연도 (2009)

void load_names(FILE *fp, int start_year, LIST *list) {
    int year, frequency;
    char sex, name[20], buffer[100];
    tName *newName;
    NODE *pPre, *pLoc;
    int search_result;

    pPre = NULL;
    pLoc = list->head;

    while (fgets(buffer, sizeof(buffer), fp)) {
        if (sscanf(buffer, "%d %s %c %d", &year, name, &sex, &frequency) != 4) {
            // Parsing failed, handle the error or skip the line.
            continue;
        }

        newName = createName(name, sex);

        search_result = _search(list, &pPre, &pLoc, newName);

        if (search_result == 1) {
            pLoc->dataPtr->freq[year - start_year] = frequency;
            pLoc->dataPtr->total_freq += frequency;
            destroyName(newName);
        } else {
            newName->freq[year - start_year] = frequency;
            newName->total_freq += frequency;
            _insert(list, pPre, newName);
        }
    }
}



// 이름순 리스트를 순회하며 빈도 순 리스트로 연결
void connect_by_frequency(LIST *list) {
    NODE *pPreFreq, *pLocFreq;
    NODE *pPreName, *pLocName;
    
    list->head2 = NULL;
    pPreName = NULL;
    pLocName = list->head;
    
    while (pLocName != NULL) {
        
        _search_by_freq(list, &pPreFreq, &pLocFreq, pLocName->dataPtr);
        
        _link_by_freq(list, pPreFreq, pLocName);
        
        pLocName = pLocName->link;
    }
}

// 이름 리스트를 화면에 출력
void print_names( LIST *pList, int num_year){
    NODE* pLoc = pList->head;
    int i;
    while(pLoc != NULL){
        printf("%s\t%c\t%d", pLoc->dataPtr->name, pLoc->dataPtr->sex, pLoc->dataPtr->total_freq);
        for(i=0;i<num_year;i++){
            printf("\t%d",pLoc->dataPtr->freq[i]);
        }
        printf("\n");
        pLoc = pLoc->link;
    }
}
void print_names_by_freq(LIST *pList, int num_year) {
    NODE *pLoc = pList->head2;
    int i;
    while (pLoc != NULL) {
        printf("%s\t%c\t%d", pLoc->dataPtr->name, pLoc->dataPtr->sex, pLoc->dataPtr->total_freq);
        for (i = 0; i < num_year; i++) {
            printf("\t%d", pLoc->dataPtr->freq[i]);
        }
        printf("\n");
        pLoc = pLoc->link2;
    }
}


////////////////////////////////////////////////////////////////////////////////
// compares two names in name structures
// for _search function
// 정렬 기준 : 이름(1순위), 성별(2순위)
static int compare_by_name(const tName *pName1, const tName *pName2)
{
	int ret = strcmp( pName1->name, pName2->name);
	if (ret != 0) return ret;

	return pName1->sex - pName2->sex;
}

// for _search_by_freq function
// 정렬 기준 : 빈도 내림차순(1순위), 이름(2순위), 성별(3순위)
static int compare_by_freq(const tName *pName1, const tName *pName2)
{
	int ret = pName2->total_freq - pName1->total_freq;
	if (ret != 0) return ret;
	
	return compare_by_name(pName1, pName2);
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	LIST *list;
	int option;
	FILE *fp;
	
	if (argc != 3)
	{
		fprintf( stderr, "Usage: %s option FILE\n\n", argv[0]);
		fprintf( stderr, "option\n\t-n\t\tsort by name\n\t-f\t\tsort by frequency\n");
		return 1;
	}

	if (strcmp( argv[1], "-n") == 0) option = SORT_BY_NAME;
	else if (strcmp( argv[1], "-f") == 0) option = SORT_BY_FREQ;
	else {
		fprintf( stderr, "unknown option : %s\n", argv[1]);
		return 1;
	}

	if ((fp = fopen( argv[2], "r")) == NULL) 
	{
		fprintf( stderr, "cannot open file : %s\n", argv[2]);
		return 1;
	}
	
	// creates an empty list
	list = createList();
	if (!list)
	{
		printf( "Cannot create list\n");
		return 100;
	}
	
	// 입력 파일로부터 이름 정보를 리스트에 저장 (이름순 리스트 구축)
	load_names( fp, 2009, list);
	
	fclose( fp);

	if (option == SORT_BY_NAME) {
		
		// 이름순 리스트를 화면에 출력
		print_names( list, MAX_YEAR_DURATION);
	}
	else { // SORT_BY_FREQ
	
		// 빈도순 리스트 연결
		connect_by_frequency( list);
		
		// 빈도순 리스트를 화면에 출력
		print_names_by_freq( list, MAX_YEAR_DURATION);
	}
	
	// 이름 리스트 메모리 해제
	destroyList( list);
	
	return 0;
}



