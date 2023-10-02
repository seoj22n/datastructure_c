#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_YEAR_DURATION	10	// 기간
#define SORT_BY_NAME	0 // 이름 순 정렬
#define SORT_BY_FREQ	1 // 빈도 순 정렬

// 구조체 선언
// 이름 구조체
typedef struct {
	char	name[20];		// 이름
	char	sex;			// 성별 'M' or 'F'
	int		total_freq;		// 연도별 빈도 합
	int		freq[MAX_YEAR_DURATION]; // 연도별 빈도
} tName;

// 헤더 구조체
typedef struct {
	int		len;		// 배열에 저장된 이름의 수
	int		capacity;	// 배열의 용량 (배열에 저장 가능한 이름의 수)
	tName	*data;		// 이름 구조체 배열의 포인터
} tNames;

////////////////////////////////////////////////////////////////////////////////
// 함수 원형 선언(declaration)

// 이름 순으로 정렬된 입력 파일을 읽어 이름 정보(이름, 성별, 빈도)를 이름 구조체에 저장
// 이미 구조체에 존재하는(저장된) 이름은 해당 연도의 빈도만 저장
// 새로 등장한 이름은 구조체에 추가
// 주의사항: 동일 이름이 남/여 각각 사용될 수 있으므로, 이름과 성별을 구별해야 함
// names->capacity는 1000으로부터 시작하여 1000씩 증가 (1000, 2000, 3000, ...)
// start_year : 시작 연도 (2009)
void load_names( FILE *fp, int start_year, tNames *names){
	char name_prev[20], name_cur[20];
	char sex_prev, sex_cur;
	int year, frequency;

	while(!feof(fp)){
		fscanf(fp, "%d %s %c %d", &year, name_cur, &sex_cur, &frequency);
		// 맨 처음의 사람의 정보 입력 받기
		if(names->len == 0){
			names->len++;
			strcpy(name_prev, name_cur);
			strcpy((names->data + (names->len - 1))->name, name_cur);
			sex_prev = sex_cur;
			(names->data + (names->len - 1))->sex = sex_cur;
			memset((names->data + (names->len - 1))->freq, 0, sizeof(names->data->freq));
			(names->data + (names->len - 1))->freq[year - 2009] = frequency;
			(names->data + (names->len - 1))->total_freq = 0;
			(names->data + (names->len - 1))->total_freq += frequency;
		} 
		else{
			// 사람 이름이 동일할 떄
			if(!strcmp(name_prev, name_cur)){
				// 사람 이름도 같고 성별도 동일할 때
				if(sex_prev == sex_cur){
					(names->data + (names->len - 1))->freq[year - 2009] = frequency;
					(names->data + (names->len - 1))->total_freq += frequency;
				}
				// 사람 이름은 같은데 성별이 다를 떄
				else{
					sex_prev = sex_cur;
					names->len++;
					strcpy((names->data + (names->len - 1))->name, name_cur);
					(names->data + (names->len - 1))->sex = sex_cur;
					memset((names->data + (names->len - 1))->freq, 0, sizeof(names->data->freq));
					(names->data + (names->len - 1))->freq[year - 2009] = frequency;
					(names->data + (names->len - 1))->total_freq = 0;
					(names->data + (names->len - 1))->total_freq += frequency;
				} 
			}
			// 사람 이름이 동일하지 않을 떄
			else{
				sex_prev = sex_cur;
				strcpy(name_prev, name_cur);
				names->len++;
				strcpy((names->data + (names->len - 1))->name, name_cur);
				(names->data + (names->len - 1))->sex = sex_cur;
				memset((names->data + (names->len - 1))->freq, 0, sizeof(names->data->freq));
				(names->data + (names->len - 1))->freq[year - 2009] = frequency;
				(names->data + (names->len - 1))->total_freq = 0;
				(names->data + (names->len - 1))->total_freq += frequency;
			}
			// name->len이 names->capacity와 동일할 때
			if (names->len == names->capacity)
				{
					names->capacity += 1000;
					names->data = realloc(names->data, names->capacity * sizeof(tName));
				}
		}
	}
}

// 구조체 배열을 화면에 출력 (이름, 성별, 빈도 합, 연도별 빈도)
void print_names( tNames *names, int num_year){ // num_year = max_duration
	int i, j;
	for(i=0;i<names->len;i++){
		printf("%s\t", ((names->data)+i)->name);
		printf("%c\t", ((names->data)+i)->sex);
		printf("%d\t", ((names->data)+i)->total_freq);
		for(j=0;j<MAX_YEAR_DURATION;j++){
			printf("%d\t", ((names->data)+i)->freq[j]);}
		printf("\n");
	}
}

// qsort를 위한 비교 함수
// 정렬 기준 : 이름(1순위), 성별(2순위)
int compare_by_name( const void *n1, const void *n2){
	if(!strcmp(((tName*)n1)->name, ((tName*)n2)->name)){
		if(((tName*)n1)->sex <((tName*)n2)->sex)	return -1;
		else return 1;
	}
	else return strcmp(((tName*)n1)->name, ((tName*)n2)->name);
}

// 정렬 기준 : 빈도 내림차순(1순위), 이름(2순위), 성별(3순위)
int compare_by_freq( const void *n1, const void *n2){
	if(((tName*)n1)->total_freq == ((tName*)n2)->total_freq){
		if(!strcmp(((tName*)n1)->name, ((tName*)n2)->name)){
			return strcmp(&((tName*)n1)->sex, &((tName*)n2)->sex);
		}
		else return strcmp(((tName*)n1)->name, ((tName*)n2)->name);
	}
	else{
		if(((tName*)n1)->total_freq > ((tName*)n2)->total_freq)
			return -1;
		else return 1;
	}
}

////////////////////////////////////////////////////////////////////////////////
// 함수 정의 (definition)

// 이름 구조체를 초기화
// len를 0으로, capacity를 1000으로 초기화
// return : 구조체 포인터
tNames *create_names(void)
{
	tNames *pnames = (tNames *)malloc( sizeof(tNames));
	
	pnames->len = 0;
	pnames->capacity = 1000;
	pnames->data = (tName *)malloc(pnames->capacity * sizeof(tName));

	return pnames;
}

// 이름 구조체에 할당된 메모리를 해제
void destroy_names(tNames *pnames)
{
	free(pnames->data);
	pnames->len = 0;
	pnames->capacity = 0;

	free(pnames);
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	tNames *names;
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
	
	// 이름 구조체 초기화
	names = create_names();

	if ((fp = fopen( argv[2], "r")) == NULL) 
	{
		fprintf( stderr, "cannot open file : %s\n", argv[2]);
		return 1;
	}

	// 입력 파일(이름 정보)을 구조체에 저장
	load_names( fp, 2009, names);

	fclose( fp);

	if (option == SORT_BY_NAME) {
		// 정렬 (이름순 (이름이 같은 경우 성별순))
		qsort( names->data, names->len, sizeof(tName), compare_by_name);
	}
	else { // SORT_BY_FREQ
		// 정렬 (빈도순 (빈도가 같은 경우 이름순))
		qsort( names->data, names->len, sizeof(tName), compare_by_freq);
	}
		
	// 이름 구조체를 화면에 출력
	print_names( names, MAX_YEAR_DURATION);

	// 이름 구조체 해제
	destroy_names( names);
	
	return 0;
}