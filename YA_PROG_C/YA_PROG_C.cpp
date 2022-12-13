#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h> //FILE, fopen(), fclose(), fseek(), ftell(), fread(), printf(), gets(), getchar(), fflush()
#include <stdlib.h> // malloc()

struct priorNode
{
	int ascii;
	int sum;
	priorNode* next;
	priorNode* prev;
} *priorHead;

priorNode* createQue(priorNode*& priorQue, int ascii, int sum);
void deleteQue(priorNode*& priorQue);
void sortQue(int numUsed);

int main()
{
	FILE* file;
	int fileLen;
	char a;
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
		fread(&a, sizeof(char), 1, file);	// находим кол-во символов
		printf("%c", a);
		flags[a]++;
	}
	printf("\n");

	struct priorNode* priorTemp = (struct priorNode*)malloc(numUsed * sizeof(struct priorNode));
	
	//priorNode* priorQue = NULL;
	for (int i = 0, j = 0; i < 256; i++)
	{
		if (flags[i] != 0)
		{
//			priorQue = createQue(priorQue, i, flags[i]);
			priorTemp[j].ascii = i;
			priorTemp[j].sum = flags[i];
			j++;
			numUsed++;
			printf("%d. %c. = %d\n", i, i, flags[i]);
		}
	}
	//printf("%d\n", priorHead);
	//printf("%d\n", priorQue);
	printf("%d", numUsed);

	//priorQue = priorHead;


	for (int i = 0; i < numUsed; i++)
	{
		printf("%d, %d\n", priorTemp[i].ascii, priorTemp[i].sum);
		
		//printf("%c, %d\n", priorQue->ascii, priorQue->sum);
		//priorQue = priorQue->next;
	}
	//sortQue(numUsed);
	//priorQue = priorHead;
	//for (int i = 0; i < numUsed; i++)
	//{
	//	printf("%c, %d\n", priorQue->ascii, priorQue->sum);
	//	priorQue = priorQue->next;
	//}





	//deleteQue(priorHead);
	fclose(file);
	getchar();
	return 0;
}


priorNode *createQue(priorNode *&priorQue, int ascii, int sum)
{

	if (NULL == priorQue)
	{
		priorQue = (priorNode*)malloc(sizeof(priorNode));
		priorQue->ascii = ascii;
		priorQue->sum = sum;
		priorQue->prev = NULL;
		priorQue->next = NULL;
		priorHead = priorQue;
		return priorQue;
	}
	else
	{
		priorQue->next = (priorNode*)malloc(sizeof(priorNode));
		priorQue->next->ascii = ascii;
		priorQue->next->sum = sum;
		priorQue->next->next = NULL;
		priorQue->prev = priorQue;
		return priorQue->next;
	}
}

void deleteQue(priorNode *&priorQue)
{
	if (priorQue != NULL)		// если узел не пуст
	{
		deleteQue(priorQue->next);	// функция вызывает саму себя с адресом левой ветви
		free(priorQue);		// освобождаем ранее выделенную память
	}
}

void sortQue(int numUsed)
{
	priorNode *priorQue;
	priorQue = priorHead;
	priorNode *priorTemp = (priorNode*)malloc(sizeof(priorNode));
	for (int i = 0; i <= numUsed; i++)
	{
		for (int j = 1; j > i; j--)
		{
			if (priorQue->sum > priorQue->next->sum)
			{
				priorTemp->sum = priorQue->sum;
				priorTemp->ascii = priorQue->ascii;
				priorQue->sum = priorQue->next->sum;
				priorQue->ascii = priorQue->next->ascii;
				priorQue->next->sum = priorTemp->sum;
				priorQue->next->ascii = priorTemp->ascii;
			}
		}
	}
	free(priorTemp);
}
