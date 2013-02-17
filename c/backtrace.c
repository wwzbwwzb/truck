/*
	 1. to control a process
	 ptrace
keyword:
   credentials, capbilities,
   ptrace,ptrctl, 
scope
	sysctl.yama.ptrace_scope
	ptrctl ( capbilities
	prctl (ptrace



	?cpu affinity
	? malicious 
	 */

/*
	 credentials and capbiliteis,
	 */
/*
	man  pthreads , 
	 erros 
	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <execinfo.h>
void dump_trace(void)
{
	int j, nptrs;
#define SIZE 100
	void *buffer[100];
	char **strings;

	nptrs = backtrace(buffer, SIZE);
	printf("backtrace() returned %d addresses\n", nptrs);

	strings = backtrace_symbols(buffer, nptrs);
	if (strings == NULL) {
		perror("backtrace_symbols");
		exit(EXIT_FAILURE);
	}

	for (j = 0; j < nptrs; j++)
		printf("%s\n", strings[j]);

	free(strings);
	/* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
		 would produce similar output to the following: */

}

	/* use -rdynamic to export this symbol */
	void   /* "static" means don't export the symbol... */
 __attribute__((visibility("hidden"))) /* hidden will be override  by rdynamic option*/
 myfunc2(void)
{
//	dump_trace();
	*(int*)0xdeadbadd=40;
}

	void
myfunc(int ncalls)
{
	if (ncalls > 1)
		myfunc(ncalls - 1);
	else
		myfunc2();
}

#include <signal.h>
void handler(int signal)
{
	printf("===> handle segfault \n");
	dump_trace();
	exit(3);
}

	int
main(int argc, char *argv[])
{
signal(SIGSEGV, handler);

	if (argc != 2) {
		fprintf(stderr, "%s num-calls\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	myfunc(atoi(argv[1]));
	exit(EXIT_SUCCESS);
}
