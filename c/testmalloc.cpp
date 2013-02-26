
#include <stdio.h>


int main(int argc, char *argv[])
{
	int **myarray ;
	int i,j;
	
	printf("argc=%d\n", argc);
	for ( int i = 0; i< 1000; i++){
		
		myarray = new int * [1024];
		for ( j =0; j<1024; j++)
			myarray[j] = new int [ 1024];
		for (j=0; j<1024; j++)
			delete [] myarray[j];
		delete [] myarray;
	}
	return 0;
}
