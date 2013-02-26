
#include <stdio.h>

void _init()
{
	printf(" this is in _init(), err=%s\n", dlerror());
}

void test()
{
	printf(" this is in test(), err=%s\n", dlerror());
	call();
}

void _fini()
{
	printf(" this is in _fini(), err=%s\n", dlerror());
}

