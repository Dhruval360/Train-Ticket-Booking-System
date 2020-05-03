#include<stdio.h>
int main()
	{
	FILE *acc = fopen("Databases/accounts", "wb");
	fclose(acc);
	return 0;
	}
