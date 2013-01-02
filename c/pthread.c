
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

	pthread_key_t key;
void on_exit2( void *arg)
{
	printf("   ***on exit(thread=0x%x): arg=%p\n", (int)pthread_self(),  arg);
}

void *thread( void *arg)
{
	//must call setspecific to enable thread cleaner
	//If TLS is empty, cleaner will not be called
	pthread_setspecific(key, (void*)3);
	printf(" **in thread %x getspecif=%p\n", (int)pthread_self(),
			pthread_getspecific(key));
	sleep(2);

}

void __attribute((destructor)) main_exit()
{
	printf("*** main exit\n");
}

int main()
{
	pthread_t pid;
	void *result;
	pthread_key_create(&key, on_exit2);
	printf("*** create key %d \n", (int)key);
	printf("*** pid=%d \n", (int)getpid());

	pthread_create(&pid, NULL, thread, NULL);

	pthread_setspecific(key, (void*)3);
	//??? why only child thread can call cleaner
	printf("thread %x \n", (int)pthread_self());

	//thread sync is needed
	pthread_join(pid, &result);

	//??? if interrupte by signal ,cleaner and destructor will not be called
	return 0;
}

