
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void on_exit2( void *arg)
{
	printf("   ***on exit(thread=0x%x): arg=%p\n", (int)pthread_self(),  arg);
}

void __attribute((destructor)) main_exit()
{
	printf("*** main exit\n");
}

int main()
{
	void *result;
	static pthread_key_t key=43;
	int value;
	pthread_key_create(&key, on_exit2);
	pthread_key_create(&key, on_exit2);
	pthread_key_create(&key, on_exit2);
	pthread_key_create(&key, on_exit2);
	pthread_key_create(&key, on_exit2);
	printf("*** create key %d \n", (int)key);
	printf("*** pid=%d \n", (int)getpid());
	printf("thread %x \n", (int)pthread_self());

	pthread_setspecific(key, (void*)3);
	value = 			pthread_getspecific(key);
	printf(" **in thread getspecif=%d\n", value);

	//thread sync is needed

	//??? if interrupte by signal ,cleaner and destructor will not be called
	return 0;
}

