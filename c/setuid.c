   #include <sys/types.h>
       #include <unistd.h>


#include <stdio.h>

int main()
{
	int ret= seteuid(0);

	printf("seteuid ret=%d %m\n",ret);

	system("echo hello gwj > /dev/console");
	printf("gwj test print to stdout stderr\n");
	printf("\033[31;1m gwj this is print in stdout\033[0m\n");
	fprintf(stderr, "\033[31;1m  gwj this is print in stdout\033[0m\n");
	return 0;
}
