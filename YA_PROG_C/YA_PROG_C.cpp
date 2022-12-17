#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h> //FILE, fopen(), fclose(), fseek(), ftell(), fread(), printf(), gets(), getchar(), fflush()
#include <stdlib.h> // malloc()
#include <string.h>

struct charCount {
	int sum = 0;
	int ascii = 0;
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

int tableRow = 0; // for tree codding
int curPosition = 0;


char FILENAME[255];
char mode = '0';
int numUsed = 0;


FILE* getFileName();
void getCharacterFrequency(charCount*& flags, int fileLen, FILE* file, priorNode*& priorQue);
priorNode* createQue(priorNode* priorQue, int ascii, int sum);
void sortQue();
//void printQue(priorNode* priorQue);
void priorTree(priorNode* priorQue);
void priorInsert(priorNode* priorQue, priorNode* pNew);
void treeCodding(priorNode* priorQue, char* code);
void fileCodding(FILE* file, int fileLen, int numUsed);
void fileUncoding(FILE* file, int byteCount, int bytePart, int numUsed, char* &codedFile);
void priorWalking(priorNode*& priorQue, char* &codedFile, FILE* &file);
void priorDelete(priorNode*& priorQue);
void getSortedFlags(FILE* file);

int main()
{
	FILE* file = getFileName();
	if (mode == '1')													// to ZIP
	{
		fseek(file, 0, SEEK_END);
		int fileLen = ftell(file);										// file length
		fseek(file, 0, SEEK_SET);
		flags = (charCount*)malloc(256 * sizeof(charCount));			// character frequency table
		priorNode* priorQue = NULL;										// queue of nodes
		getCharacterFrequency(flags, fileLen, file, priorQue);			// fill character frequency table and create a queue of nodes
		free(flags);
		sortQue();														// sorting queue of nodes
		priorTree(priorHead);											// building the tree

		pCoder = (codeTable*)malloc(numUsed * sizeof(codeTable));		// keep ascii and its code
		for (int i = 0; i < numUsed; i++)
			pCoder[i].ascii = 0;
		char code[8] = "";												// for tree codding, keeps codes before put it in pCoder table
		treeCodding(priorHead, code);
		priorDelete(priorHead);											// delete tree
		fileCodding(file, fileLen, numUsed);							// cooding file
		fclose(file);
		free(pCoder);
		getchar();
	}
	else if (mode == '0')												// to UNZIP
	{
		int byteCount = 0;													
		int bytePart = 0;													
		fread(&numUsed, 1, 1, file);
		fread(&byteCount, sizeof(int), 1, file);
		fread(&bytePart, 1, 1, file);
		flags = (charCount*)malloc(numUsed * sizeof(charCount));		// keep ascii and its code
		getSortedFlags(file);
		priorNode* priorQue = NULL;										// queue of nodes
		for (int i = 0; i < numUsed; i++)
		{
			priorQue = createQue(priorQue, flags[i].ascii, flags[i].sum);
			printf("flags: %d. %c. = %d\n", i, flags[i].ascii, flags[i].sum);

		}
		free(flags);
		sortQue();														// sorting queue of nodes
		priorTree(priorHead);											// building the tree

		pCoder = (codeTable*)malloc(numUsed * sizeof(codeTable));		// keep ascii and its code
		for (int i = 0; i < numUsed; i++)
			pCoder[i].ascii = 0;
		char code[8] = "";												// for tree codding, keeps codes
		treeCodding(priorHead, code);
		char* codedFile = (char*)malloc((byteCount * 8 + bytePart + 1) * sizeof(char));		// keeps all file data in chars
		fileUncoding(file, byteCount, bytePart, numUsed, codedFile);						// getting data from file
		printf("CodedFILE is: %s\n", codedFile);
		int sizeLeft = strlen(codedFile);
		FILE* file2;
		if (!(file2 = fopen(FILENAME, "w+b")))
		{
			printf("\nCannot create file2\n");
			getchar();
			exit(0);
		}
		do 
		{
			priorWalking(priorHead, codedFile, file2);
		} while (curPosition < sizeLeft);

		priorDelete(priorHead);											// delete tree
		fclose(file);
		free(codedFile);
		getchar();
	}
	else
	{
		printf("Unknown error\n");
		getchar();
	}
	return 0;
}

FILE* getFileName()
{
	FILE* file;
	char filename[255];
	printf("Enter the file name: ");
	gets(filename);
	fflush(stdin);
	if (!(file = fopen(filename, "r+b")))					
	{
		printf("\nFile doesn't exists.");				
		getchar();
		exit(0);
	}
	int nameLen = strlen(filename);
	int namePosition = 0;
	int extentionPosition = 0;
	char extention[10];
	for (int i = nameLen; i >= 0; i--)
	{
		if ((filename[i] == '.') && (mode == '0'))
		{
			extentionPosition = i;
			mode = '1';
		}
		if (filename[i] == '\\')
		{
			namePosition = i + 1;
			break;
		}
	}
	int j = 0;
	for (int i = namePosition; i < nameLen; i++)
	{
		FILENAME[j] = filename[i];
		j++;
	}
	FILENAME[j] = '\0';
	j = 0;
	for (int i = extentionPosition; i < nameLen; i++)
	{
		extention[j] = filename[i];
		j++;
	}
	extention[j] = '\0';
	if (strcmp(extention, ".ivs51") == 0)
		mode = '0';
	return file;
}

void getCharacterFrequency(charCount* &flags, int fileLen, FILE *file, priorNode* &priorQue)
{
	char character;
	for (int i = 0; i < 256; i++)
		flags[i].sum = flags[i].ascii = 0;
	for (int i = 0; i < fileLen; i++)
	{
		fread(&character, sizeof(char), 1, file);
		printf("%c", character);
		if (flags[character].sum == 0)
		{
			flags[character].ascii = i;
			numUsed++;
		}
		flags[character].sum++;
	}
	for (int i = 0; i < 256; i++)
	{
		if ((flags[i].sum > 0))
		{
			priorQue = createQue(priorQue, i, flags[i].sum);
			printf("flags: %d. %c. = %d\n", i, i, flags[i].sum);
		}
	}
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

/*void printQue(priorNode* priorQue)
{
	struct priorNode* pTemp;
	pTemp = priorQue;
	do
	{
		printf("ascii:%d, char: %c, sum:%d\n", pTemp->ascii,pTemp->ascii, pTemp->sum);
		pTemp = pTemp->next;
	} while (pTemp != priorTail->next);
	return;
}*/

void priorTree(priorNode* priorQue)
{
	while (priorHead != priorTail->next)
	{
		priorNode* pNew = (priorNode*)malloc(sizeof(priorNode));
		pNew->sum = (priorQue->sum + priorQue->next->sum);
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
			priorInsert(priorHead, pNew);
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
//		printf("i go right\n");
		treeCodding(priorQue->right, code);
		if (strlen(code) > 0)
			code[strlen(code) - 1] = '\0';
		
	}
	if (priorQue->left != NULL)
	{
		strcat(code, "0");
//		printf("i go left\n");
		treeCodding(priorQue->left, code);
		if (strlen(code) > 0)
			code[strlen(code) - 1] = '\0';
	}
	if (priorQue->right == NULL && priorQue->left == NULL)
	{
		strcpy(priorQue->code, code);
		strcpy(pCoder[tableRow].code, priorQue->code);
		pCoder[tableRow].sum = priorQue->sum;
		pCoder[tableRow].ascii = priorQue->ascii;
		tableRow++;
//		printf("askii i have: %c\n", priorQue->ascii);
	}
//	printf("i return up:\n");
	return;
}

void fileCodding(FILE* file, int fileLen, int numUsed)
{
	char character;
	char* codedFile = (char*)malloc(fileLen * 5 * sizeof(char));		// keeps all file data in chars
	strcpy(codedFile, "");
	int byteCount = 0;													// amount of bytes needs to code
	int bytePart = 0;													// amount of bites left
	fseek(file, 0, SEEK_SET);
	for (int i = 0; i < fileLen; i++)
	{
		fread(&character, sizeof(char), 1, file);						// coding each character from file
		for (int j = 0; j < numUsed; j++)
		{
			if ((int)character == pCoder[j].ascii)
				strcat(codedFile, pCoder[j].code);
		}
	}
	byteCount = strlen(codedFile) / CHAR_BIT;
	bytePart = strlen(codedFile) % CHAR_BIT;
	FILE* file2;														// coded file
	char filename[255];
	strcpy(filename, FILENAME);											// coded file name
	strcat(filename, ".ivs51");
	if (!(file2 = fopen(filename, "w+b")))
	{
		printf("\nCannot create a file");
		getchar();
		return;
	}
	else
	{																	// FILE CONTAINS:
		fwrite(&numUsed, sizeof(char), 1, file2);						// 1 byte: number of original symbols used
		fwrite(&byteCount, sizeof(int), 1, file2);						// 2-5 byte: number of bytes used for coded data
		fwrite(&bytePart, sizeof(char), 1, file2);						// 6 byte: number of bites left
		for (int i = 0; i < numUsed; i++)
		{																// character frequency table:
			fwrite(&pCoder[i].ascii, sizeof(char), 1, file2);		// (first: char code(1byte), second: amount of 
			fwrite(&pCoder[i].sum, sizeof(int), 1, file2);			// frequency(4bytes)) * number of original symbols
		}
		unsigned char codedByte = NULL;									// coded data
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
		for (int j = 0; j < 8; j++)										// coded bites in 1 byte
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

void fileUncoding(FILE* file, int byteCount, int bytePart, int numUsed, char* &codedFile)
{
	unsigned char codedByte = NULL;
	int k = 0;
	for (int i = 0; i < byteCount; i++)
	{
		fread(&codedByte, 1, 1, file);
		for (int j = 0; j < 8; j++)
		{
			if (codedByte & 128)
				codedFile[k] = '1';
			else
				codedFile[k] = '0';
			codedByte <<= 1;
			k++;
		}
	}
	fread(&codedByte, 1, 1, file);
	k = k + bytePart - 1;
	for (int j = 0; j < bytePart; j++)
	{
		if (codedByte & 1)
			codedFile[k] = '1';
		else
			codedFile[k] = '0';
		codedByte >>= 1;
		k--;
	}
	k = k + bytePart + 1;
	codedFile[k] = '\0';
	return;
}

void priorDelete(priorNode*& priorQue)
{
	if (priorQue != NULL)
	{
		priorDelete(priorQue->right);
		priorDelete(priorQue->left);
		free(priorQue);
	}
	return;
}
void priorWalking(priorNode*& priorQue, char*& codedFile, FILE*& file)
{
	if (codedFile[curPosition] == '1' && priorQue->right != NULL)
	{
		curPosition++;
		priorWalking(priorQue->right, codedFile, file);
	}
	else if ((codedFile[curPosition] == '1'|| codedFile[curPosition] == '\0') && priorQue->right == NULL)
	{
		printf("%c", priorQue->ascii);
		fwrite(&priorQue->ascii, 1, 1, file);
		return;
	}
	else if (codedFile[curPosition] == '0' && priorQue->left != NULL)
	{
		curPosition++;
		priorWalking(priorQue->left, codedFile, file);
	}
	else if ((codedFile[curPosition] == '0' || codedFile[curPosition] == '\0') && priorQue->left == NULL)
	{
		printf("%c", priorQue->ascii);
		fwrite(&priorQue->ascii, 1, 1, file);
		return;
	}

	return;
}
void getSortedFlags(FILE* file)
{
	for (int i = 0; i < numUsed; i++)
	{
		fread(&flags[i].ascii, 1, 1, file);
		fread(&flags[i].sum, sizeof(int), 1, file);
	}
	int ascii, sum = 0;
	for (int i = 0; i < numUsed - 1; i++)
	{
		for (int j = numUsed; j > i; --j)
		{
			if (flags[j - 1].ascii > flags[j].ascii)
			{
				printf("sorting\n");
				ascii = flags[j].ascii;
				sum = flags[j].sum;
				flags[j].ascii = flags[j - 1].ascii;
				flags[j].sum = flags[j - 1].sum;
				flags[j - 1].ascii = ascii;
				flags[j - 1].sum = sum;
			}
		}
	}
}
