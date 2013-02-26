#include <stdio.h>
#include <dlfcn.h>

void hello(int a)
{
  printf("call hello in .exe :%d\n", a);
}

int func1(int a)
{
    hello(a);
}

int main()
{
  printf("excutable running....\n");

  func1(3);

  void *h = dlopen("/home/gwj/b.so", RTLD_LAZY | RTLD_GLOBAL);
  if (!h)
    printf("%s\n", dlerror());

  return 0;
}
// b.so source code: 
#include <stdio.h>


int func2(int a)
{
    hello(a);
}

//  _init() can't be override by -nostdlib option for gcc, 
// but you can define multi _init() by use gcc options  __attribute__((constructor)) 
// and __attribute__((destructor))
// it work well 
void   __attribute__((constructor)) _init_dl()
{
  printf(" calling _init\n");
  func2(4);
}

void   __attribute__((destructor))  _fini_dl()
{
  printf(" calling _exit\n");
  func2(5);
}
//built with this command : gcc -shared b.c -o b.so ; gcc a.c -ldl -rdynamic ; ./a.out 
