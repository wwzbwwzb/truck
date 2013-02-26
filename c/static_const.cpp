#include <stdio.h>

namespace testA {

class A 
{
public:	
	enum config_part{
		PARTA=1,
		PARTB=2,
	};
	mutable	int a;
	void change() const {
		a=32;
	}
	static void change2(){
		int a;
		a=33;
	}
};
}; //namespace testA
using namespace testA;
namespace testB {

	A::config_part gts;

int test_main()
{
	A a;
	A::config_part configa=A::PARTA;
	int cnt=0;
	//A::change();
	//configa=A::PARTA;
	a.change();
	printf(" a= %d\n", a.a);
	printf("%d\n", A::PARTA);
	printf("%d\n", configa);

	for(int i=0; i< 10000000; i++){
		char * st= new char [2048];
		printf(" cnt=%d\n", cnt+=2);
//		delete st;
	}
	getchar();

	return 0;
}
}; //namespace testB

using namespace testB;
//typedef list<int> ALSAHandleList;
int main() {
return 	test_main();
}
