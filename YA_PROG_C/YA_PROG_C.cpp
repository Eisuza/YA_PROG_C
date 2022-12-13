#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h> //FILE, fopen(), fclose(), printf(), gets(), getchar(), fflush()

int main()
{
	FILE *file;
	int fileLen;
	int a;
	unsigned char flags[256] = { 0 };
	char filename[255];
	printf("Enter the file name to archive: ");
	gets(filename);
	fflush(stdin);
	if (!(file = fopen(filename, "r+b")))					// ���� ���� �� ������/�� ������
	{
		printf("\nFile doesn't exists.");				// ������� ���������
		getchar();
		return 0;
	}
	printf("\n all is fine\n");
	fseek(file, 0, SEEK_END);				// ��������� ����� �����
	fileLen = ftell(file);					// �������� ���-�� �������� � �����
	fseek(file, 0, SEEK_SET);				// ������������ � ������ �����
	printf("Characters which we got from the file are: \n");
	for (int i = 0; i < fileLen; i++)
	{
		fread(&a, sizeof(char), 1, file);	// ��������� �� ����� 1 ������
		printf("%c ", a);					// ������� ��� �� ��������
		flags[a] += 1;
	}
	printf("\n");
	for (int i = 0; i < 256; i++)
	{
		if (i == 7)
			printf("%d.  = Bell\n", i);
		else
			printf("%d. %c. = %d\n", i, i, flags[i]);
	}

	fclose(file);
	getchar();
	return 0;
}