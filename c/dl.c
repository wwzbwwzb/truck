       #include <curses.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <errno.h>
#include <string.h>


int call()
{
	printf(" incall \n");
	return 0;
}


int main()
{
//	printf("err:%s\n", strerror(2));
	char dlname[] = "/home/gwj/share/vlc/lib/plugins/libmemcpymmx_plugin.so";
	char dlname2[] = "libvlccore.so.5";
	//void *dh = dlopen(dlname, RTLD_NOW);
	void *dh = dlopen(dlname, RTLD_LAZY);
	if (!dh){
		perror("dlopen");
		printf("errno=%d\n",errno);
		printf("dlerror[%s]\n", dlerror());
	}
		return 0;

}

