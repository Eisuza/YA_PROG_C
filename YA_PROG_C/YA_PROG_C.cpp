#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h> //FILE, fopen(), fclose(), fseek(), ftell(), fread(), printf(), gets(), getchar(), fflush()
#include <stdlib.h> // malloc()
#include <string.h>

struct charCount {
	int sum = 0;
	int priory = 0;
} *flags;

struct priorNode
{
	int ascii = 0;
	int sum = 0;
	char code[16];
	priorNode* prev = NULL;		// for que
	priorNode* next = NULL;
	priorNode* parent = NULL;	// for tree
	priorNode* left = NULL;
	priorNode* right = NULL;
} *priorHead, *priorTail;

struct codeTable
{
	char code[16];
	int ascii;
	int sum;
} *pCoder;

int tableRow = 0;

priorNode* createQue(priorNode* priorQue, int ascii, int sum);
void sortQue();
void printQue(priorNode* priorQue);
void priorTree(priorNode* priorQue);
void priorInsert(priorNode* priorQue, priorNode* pNew);
void treeCodding(priorNode* priorQue, char* code);
void fileCodding(codeTable* pCoder, FILE* file, int fileLen, int numUsed, charCount* flags);
void priorDelete(priorNode*& priorQue);

int main()
{
	FILE* file;
	int fileLen;
	char character;
	int numUsed = 0;
	flags = (charCount*)malloc(256*sizeof(charCount));
	for (int i = 0; i < 256; i++)
		flags[i].sum = flags[i].priory = 0;
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
	printf("%d", flags[25].sum);
	fseek(file, 0, SEEK_END);				// достигаем конца файла
	fileLen = ftell(file);					// получаем кол-во символов в файле
	fseek(file, 0, SEEK_SET);				// возвращаемся в начало файла
	printf("fileLen: %d\n", fileLen);
	for (int i = 0, j = 1; i < fileLen; i++)
	{
		fread(&character, sizeof(char), 1, file);	// находим кол-во каждого из символов
		printf("%c", character);
		if (flags[character].sum == 0)
		{
			flags[character].priory = j;
			numUsed++;
			j++;
		}
		flags[character].sum++;
	}
	printf("\n");

	priorNode* priorQue = NULL; // таблица кодов
	for (int k = 0, j = numUsed; k < numUsed; k++)
	{
		for (int i = 0; i < 256; i++)
		{
			if ((flags[i].sum > 0) && (flags[i].priory == j))
			{
				priorQue = createQue(priorQue, i, flags[i].sum);
				printf("flags: %d. %c. = %d\n", i, i, flags[i].sum);
				j--;
			}
		}
	}
	printQue(priorHead);
	sortQue();
	printQue(priorHead);
	priorTree(priorHead);
	char code[8] = "";
	pCoder = (codeTable*)malloc(numUsed * sizeof(codeTable));
	for (int i; i < numUsed; i++)
		pCoder[i].ascii = 0;
	treeCodding(priorHead, code);
	priorDelete(priorHead);

	for (int i = 0; i < numUsed; i++)
		printf("Ascii: %c, Code: %s\n", pCoder[i].ascii, pCoder[i].code);

	fileCodding(pCoder, file, fileLen, numUsed, flags);

	free(flags);
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


void printQue(priorNode* priorQue)
{
	struct priorNode* pTemp;
	pTemp = priorQue;
	do
	{
		printf("ascii:%d, char: %c, sum:%d\n", pTemp->ascii,pTemp->ascii, pTemp->sum);
		pTemp = pTemp->next;
	} while (pTemp != priorTail->next);
	return;
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
		printf("value is %d\n", value);

		switch (value)
		{
		case 1:						// before head
			pNew->prev = NULL;
			pNew->next = priorQue;
			priorQue->prev = pNew;
			priorHead = pNew;
			value = 0;
			break;
		case 2:						// between
			pNew->prev = priorQue->prev;
			pNew->next = priorQue;
			priorQue->prev->next = pNew;
			priorQue->prev = pNew;
			value = 0;
			break;
		case 3:						// after tail
			pNew->next = NULL;
			pNew->prev = priorQue;
			priorQue->next = pNew;
			priorTail = pNew;
			value = 0;
			break;
		case 4:						// needs ahead
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

void treeCodding(priorNode* priorQue, char* code)	
{						
	if (priorQue->right != NULL)
	{
		strcat(code, "1");
		printf("right) code is: %s\n", code);
		treeCodding(priorQue->right, code);
		printf("vozvrat iz pravogo\n");
		if (strlen(code) > 0)
			code[strlen(code) - 1] = '\0';

	}
	if (priorQue->left != NULL)
	{
		strcat(code, "0");
		printf("left) code is: %s\n", code);
		treeCodding(priorQue->left, code);
		printf("vozvrat iz levogo\n");
		if (strlen(code) > 0)
			code[strlen(code) - 1] = '\0';
	}
	if (priorQue->right == NULL && priorQue->left == NULL)
	{
		printf("my kopiruem eto: %s\n", code);
		strcpy(priorQue->code, code);
		printf("i = %d, sum in node :%d, ascii in tree: %d, code is: %s\n", tableRow, priorQue->sum, priorQue->ascii, priorQue->code);
		strcpy(pCoder[tableRow].code, priorQue->code);
		pCoder[tableRow].sum = priorQue->sum;
		pCoder[tableRow].ascii = priorQue->ascii;
		printf("ASKII JUST COPY: %d CODE JUST COPY: %s\n", pCoder[tableRow].ascii, pCoder[tableRow].code);

		tableRow++;
	}
	return;
}

void fileCodding(codeTable* pCoder, FILE* file, int fileLen, int numUsed, charCount *flags)
{
	char character;
	char* codedFile = (char*)malloc(fileLen * 5 * sizeof(char));
	int byteCount = 0;
	int bytePart = 0;
	printf("STRLEN codedFile: %d\n", strlen(codedFile));
	printf("fileLen is: %d\n", fileLen);
	strcpy(codedFile, "");

	fseek(file, 0, SEEK_SET);				// возвращаемся в начало файла
	for (int i = 0; i < fileLen; i++)
	{
		fread(&character, sizeof(char), 1, file);	// находим кол-во каждого из символов
//		printf("%d|", character);
		for (int j = 0; j < numUsed; j++)
		{
			if ((int)character == pCoder[j].ascii)
			{
//				printf("pCoder[j].code = %s\n", pCoder[j].code);
				strcat(codedFile, pCoder[j].code);
				//printf("%s\n", codedFile);
			}
		}
	}
	printf("%s\n", codedFile);
	printf("SIZEOF codedFile: %d\n", strlen(codedFile));
	byteCount = strlen(codedFile) / CHAR_BIT;
	bytePart = strlen(codedFile) % CHAR_BIT;
	printf("byteCount needed: %d\nbytePart left: %d\n", byteCount, bytePart);
	printf("random char: %c\n", codedFile[5]);

	FILE* file2;

	char filename2[255];
	strcpy(filename2, "NewFile.ivs51");
	printf("filename2: %s", filename2);
	if (!(file2 = fopen(filename2, "w+b")))					// если файл не найден/не открыт
	{
		printf("\nFile doesn't exists.");				// выводим сообщение
		getchar();
		return;
	}
	else
	{
		fwrite(&numUsed, sizeof(char), 1, file2);
		fwrite(&byteCount, sizeof(int), 1, file2);
		fwrite(&bytePart, sizeof(char), 1, file2);
		for (int i = 0; i < 256; i++)
		{
			if ((flags[i].sum > 0))
			{
				fwrite(&i, sizeof(char), 1, file2);
				fwrite(&flags[i].sum, sizeof(int), 1, file2);
			}
		}
		unsigned char codedByte = NULL;
		int k = 0;
		for (int i = 0; i < byteCount; i++)
		{
			codedByte = NULL;
			for (int j = 0; j < 8; j++, k++)
			{
				if (codedFile[k] == '1')
				{
					codedByte = codedByte | (1 << (7 - j));
				}
			}
			fwrite(&codedByte, 1, 1, file2);
		}
		codedByte = NULL;
		for (int j = 0; j < 8; j++)
		{
			if ((j >= (8 - bytePart)) && (codedFile[k] == '1'))
			{
				codedByte = codedByte | (1 << (7 - j));
				k++;
			}
		}
			fwrite(&codedByte, 1, 1, file2);

	}
	fclose(file2);
	free(codedFile);
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
	return;
}
