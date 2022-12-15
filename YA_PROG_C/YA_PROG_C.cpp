#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h> //FILE, fopen(), fclose(), fseek(), ftell(), fread(), printf(), gets(), getchar(), fflush()
#include <stdlib.h> // malloc()
#include <string.h>


struct priorNode
{
	int ascii = 0;
	int sum = 0;
	char code[8];
	priorNode* prev = NULL;		// для очереди
	priorNode* next = NULL;
	priorNode* parent = NULL;	// для дерева
	priorNode* left = NULL;
	priorNode* right = NULL;
} *priorHead, * priorTail;

struct codeTable
{
	char code[8];
	int ascii;
} *pCoder;

int tableRow = 0;

priorNode* createQue(priorNode* priorQue, int ascii, int sum);
void sortQue();
void printQue(priorNode* priorQue);
void deleteQue(priorNode* priorQue);
void priorTree(priorNode* priorQue);
void priorInsert(priorNode* priorQue, priorNode* pNew);
void treeCodding(priorNode* priorQue, char* code);
void priorDelete(priorNode*& priorQue);
//priorNode* sortQue(int numUsed, priorNode *priorQue);
//void priorPrint(priorNode* priorQue, int numUsed);

int main()
{
	FILE* file;
	int fileLen;
	char character;
	int numUsed = 0;
	int flags[256] = { 0 };
	char filename[255];
	printf("Enter the file name to archive: ");
	gets(filename);
	fflush(stdin);
	if (!(file = fopen(filename, "r+b")))					// если файл не найден/не открыт
	{
		printf("\nFile doesn't exists.");				// выводим сообщение
		getchar();
		return 0;
	}
	printf("\n all is fine\n");
	fseek(file, 0, SEEK_END);				// достигаем конца файла
	fileLen = ftell(file);					// получаем кол-во символов в файле
	fseek(file, 0, SEEK_SET);				// возвращаемся в начало файла
	for (int i = 0; i < fileLen; i++)
	{
		fread(&character, sizeof(char), 1, file);	// находим кол-во каждого из символов
		printf("%c", character);
		if (flags[character] == 0)
			numUsed++;
		flags[character]++;
	}
	printf("\n");

	/*   FIRST VARIANT    */
/*priorNode* priorQue = (priorNode*)malloc((numUsed) * sizeof(priorNode));

for (int i = 0, j = 0; i < 256; i++)
{
	if (flags[i] > 0)
	{
		priorQue[j].ascii = i;
		priorQue[j].sum = flags[i];
		printf("priorQue:%d, %d\n", priorQue[j].ascii, priorQue[j].sum);
		j++;
		printf("flags: %d. %c. = %d\n", i, i, flags[i]);
		printf("priorQue(get) is: %d\n", priorQue);

	}
}

priorPrint(priorQue, numUsed);
priorQue = sortQue(numUsed, priorQue);
priorPrint(priorQue, numUsed);

fclose(file);
printf("priorQue(before free) is: %d\n", priorQue);
free(priorQue);*/

/*   SECOND VARIANT    */
	priorNode* priorQue = NULL;
	for (int i = 0; i < 256; i++)
	{
		if (flags[i] > 0)
		{
			priorQue = createQue(priorQue, i, flags[i]);
			printf("flags: %d. %c. = %d\n", i, i, flags[i]);
		}
	}

	printQue(priorHead);
	sortQue();
	printQue(priorHead);
	//deleteQue(priorHead);
	priorTree(priorHead);
	char code[8] = "";
	pCoder = (codeTable*)malloc(numUsed * sizeof(codeTable));
	treeCodding(priorHead, code);
	priorDelete(priorHead);

	for (int i = 0; i < numUsed; i++)
		printf("Ascii: %d, Code: %s\n", pCoder[i].ascii, pCoder[i].code);




	free(pCoder);
	printf("numUsed is: %d\n", numUsed);
	getchar();
	return 0;
}


priorNode* createQue(priorNode* priorQue, int ascii, int sum)
{

	if (priorQue == NULL)
	{
		priorQue = (priorNode*)malloc(sizeof(priorNode));
		priorQue->ascii = ascii;
		priorQue->sum = sum;
		priorQue->prev = NULL;
		priorQue->next = NULL;
		priorQue->parent = NULL;
		priorQue->left = NULL;
		priorQue->right = NULL;
		priorHead = priorQue;
		priorTail = priorQue;
		return priorQue;
	}
	else
	{
		priorQue->next = (priorNode*)malloc(sizeof(priorNode));
		priorQue->next->ascii = ascii;
		priorQue->next->sum = sum;
		priorQue->next->prev = priorQue;
		priorQue->next->next = NULL;
		priorQue->next->parent = NULL;
		priorQue->next->left = NULL;
		priorQue->next->right = NULL;
		priorTail = priorQue->next;
		return priorQue->next;
	}
}
void sortQue()
{
	priorNode* pCurrent = priorHead;
	priorNode* pTail = priorTail;
	priorNode* pRunning = priorHead->next;
	int sum = 0;
	int ascii = 0;
	while (pCurrent != pTail)
	{
		pRunning = pCurrent->next;
		while (pRunning != pTail->next)
		{
			if (pRunning->sum < pCurrent->sum)
			{
				sum = pCurrent->sum;
				ascii = pCurrent->ascii;
				pCurrent->sum = pRunning->sum;
				pCurrent->ascii = pRunning->ascii;
				pRunning->sum = sum;
				pRunning->ascii = ascii;
			}
			pRunning = pRunning->next;
		}
		pCurrent = pCurrent->next;
	}
	return;
}


void deleteQue(priorNode* priorQue) // до дерева
{
	if (priorQue != priorTail)		// если узел не пуст
		deleteQue(priorQue->next);	// функция вызывает саму себя с адресом левой ветви
	free(priorQue);		// освобождаем ранее выделенную память
	return;
}

void printQue(priorNode* priorQue)
{
	struct priorNode* pTemp;
	pTemp = priorQue;
	do
	{
		printf("ascii:%d, sum:%d\n", pTemp->ascii, pTemp->sum);
		pTemp = pTemp->next;
	} while (pTemp != priorTail->next);
}


void priorTree(priorNode* priorQue)
{
	while (priorHead != priorTail->next)
	{
		priorNode* pNew = (priorNode*)malloc(sizeof(priorNode));
		pNew->sum = (priorQue->sum + priorQue->next->sum);
		printf("sozdan pNew, on soderjit: %d + %d\n", priorQue->sum, priorQue->next->sum);
		pNew->ascii = 0;
		pNew->left = priorQue;
		pNew->right = priorQue->next;
		pNew->prev = NULL;
		pNew->next = NULL;
		pNew->parent = NULL;
		priorQue->parent = pNew;
		priorQue->next->parent = pNew;
		priorHead = priorQue->next->next;
		priorQue = priorHead;

		if (priorHead != priorTail->next)
		{
			printf("Vyzov vstavki\n");
			priorInsert(priorHead, pNew);
		}
		else
			priorQue = pNew;
	}
	priorHead = priorQue;
	return;
}
void priorInsert(priorNode* priorQue, priorNode* pNew)
{
	int value = 1;
	while (value == 1)
	{
		printf("priorQue->sum is: %d, pNew-sum is: %d\n", priorQue->sum, pNew->sum);
		if (priorQue->sum >= pNew->sum)		// New element goes before 
		{
			if (priorQue == priorHead)
				value = 1;
			else
				value = 2;
		}
		else if (priorQue->sum < pNew->sum)		// New element goes after
		{
			if (priorQue == priorTail)
				value = 3;
			else
				value = 4;
		}
		//		else
		//			value = 4;
		printf("value is %d\n", value);

		switch (value)
		{
		case 1:
			pNew->prev = NULL;
			pNew->next = priorQue;
			priorQue->prev = pNew;
			priorHead = pNew;
			value = 0;
			break;
		case 2:
			pNew->prev = priorQue->prev;
			pNew->next = priorQue;
			priorQue->prev->next = pNew;
			priorQue->prev = pNew;
			value = 0;
			break;
		case 3:
			pNew->next = NULL;
			pNew->prev = priorQue;
			priorQue->next = pNew;
			priorTail = pNew;
			value = 0;
			break;
		case 4:
			priorQue = priorQue->next;
			value = 1;
			break;
		default:
			value = 0;
			break;
		}
	}
	return;
}

void treeCodding(priorNode* priorQue, char* code)	// функция для вывода на экран и сохранения в файл В
{						// принимает ссылку на дерево и на файл (для записи в него)
	if (priorQue->right != NULL)
	{
		strcat(code, "1");
		printf("right) code is: %s\n", code);
		treeCodding(priorQue->right, code);	// функция вызывает саму себя (левая ветвь)// если текущий узел не пуст
		printf("vozvrat iz pravogo\n");
		if (strlen(code) > 0)
			code[strlen(code) - 1] = '\0';

	}
	if (priorQue->left != NULL)
	{
		strcat(code, "0");
		printf("left) code is: %s\n", code);
		treeCodding(priorQue->left, code);	// функция вызывает саму себя (правая ветвь)
		printf("vozvrat iz levogo\n");
		if (strlen(code) > 0)
			code[strlen(code) - 1] = '\0';
	}
	if (priorQue->right == NULL && priorQue->left == NULL)
	{
		printf("my kopiruem eto: %s\n", code);
		strcpy(priorQue->code, code);
		printf("i = %d, sum in node :%d, ascii in tree: %d, code is: %s\n", tableRow, priorQue->sum, priorQue->ascii, priorQue->code);
		pCoder[tableRow].ascii = priorQue->ascii;
		strcpy(pCoder[tableRow].code, priorQue->code);
		tableRow++;
	}
	return;
}

void priorDelete(priorNode*& priorQue)		// функция удаления дерева
{
	if (priorQue != NULL)		// если узел не пуст
	{
		priorDelete(priorQue->right);	// функция вызывает саму себя с адресом левой ветви
		priorDelete(priorQue->left);	// функция вызывает саму себя с адресов правой ветви
		free(priorQue);		// освобождаем ранее выделенную память
	}
}


/*priorNode* sortQue(int numUsed, priorNode* priorQue)
{
	int ascii = 0;
	int sum = 0;
	for (int i = 0; i < numUsed; i++)
	{
		for (int j = 0; j < numUsed -1; j++)
		{
			printf("compare: sum: j  = %d, i = %d\n", j, priorQue[j].sum, priorQue[i].sum);
			if (priorQue[j].sum > priorQue[i].sum)
			{
				printf("swaping\n");
				ascii = priorQue[i].ascii;
				sum = priorQue[i].sum;
				priorQue[i].ascii = priorQue[j].ascii;
				priorQue[i].sum = priorQue[j].sum;
				priorQue[j].ascii = ascii;
				priorQue[j].sum = sum;
			}
		}
	}
	return priorQue;
}
*/
/*
void priorPrint(priorNode* priorQue, int numUsed)
{
	for (int i = 0; i < numUsed; i++)
	{
		printf("%d, %d\n", priorQue[i].ascii, priorQue[i].sum);
	}
}
*/
