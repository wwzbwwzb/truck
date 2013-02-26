
struct test_struct
{
	int a;
	int b;
	char *ptr;
};

void print()
{
	printf("call in this function %s:%d\n", __FUNCTION__,  __LINE__);

}

void printr(struct test_struct *sp)
{
	printf("a=%d b=%d ptr=%s\n", sp->a, sp->b, sp->ptr);

}
