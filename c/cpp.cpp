
#include <stdio.h>
#include <iostream>
#include <vector>


int flags= 0;
struct note{
	int a;
	int b;
};

void change(int &  arg)
{
	arg = 3;
}

int main()
{
	int* test =  new int(3);
	int & test2= *test;

	change(flags);
	printf(" value =%d\n", flags);
	{
	struct note n1;
	n1.a=3;
	struct note & n2 = n1;
	printf("%d\n", n2.a);
	}

	return 0;
}

