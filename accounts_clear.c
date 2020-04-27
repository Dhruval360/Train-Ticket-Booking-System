#include<stdio.h>
int main()
	{
	FILE *acc = fopen("accounts", "wb");
	fclose(acc);
	return 0;
	}
