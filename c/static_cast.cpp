#include <stdio.h>
#include <iostream>
#include <string.h>

		// 必须申明 使用了std的成员
using std::cout;
using std::endl;

class A{
	public :
		A(){ }
		//析构函数必须是虚函数
		virtual void f(){
			printf("A:mptr=%d\n",mptr);
		}
	public:
		int mptr;
};

class B: public A 
{
	public :
		B(){ }
		void f(){
		}
		int mptr;
};

class D: public A {
	public:
		// static 不占用类的存储空间， const  表示只有一份存储的值
		static const int const1=10000;
};

/*
dynamic_cast:   通常在基类和派生类之间转换时使用；
const_cast:   主要针对const和volatile的转换.
static_cast:   一般的转换，如果你不知道该用哪个，就用这个。
reinterpret_cast:   用于进行没有任何关联之间的转换，比如一个字符指针转换为一个整形数。
android 实现了interface_cast<type-id>(p/r)
	以上运算符的type-id类型必须是pointer或者reference
	 */
int main()
{
	char str[]= "33333333333333";
	char *a= static_cast<char* >(str);
	//dynamic_cast requries class has virtual functin table(VFT)
//	char *b= dynamic_cast<char &>(str);
	printf("%s\n", a);

	//xxx_cast 
	if(0){
		B * b = new B();
		/*
		  //  Compile error	
		D *d = static_cast<D*>(b);
		*/
		D *d2 = dynamic_cast<D*>(b);
	}

	if(0){
		/* C++ 虚拟函数表的存放和处理， 一般继承，多重继承，覆盖情况，可参考
			http://blog.csdn.net/haoel/article/details/1948051
			C++ 的实现参考  《...C++对象模型》
			*/
		
		typedef void(*Fun)();
		B b;

		int *p= (int*)(&b); // static_cast 编译时会检查类型，所以更安全
		cout<<"B in memory[0] "<<(int)p[0]<<endl;	 
		cout<<"B in memory[1] "<<(int)p[1]<<endl;	
		cout<<"B in memory[2] "<<(int)p[2]<<endl;	
		cout<<"  B类的实例在内存中首先存储虚拟函数表"<<endl;
		cout<<"  然后是父类成员变量,子类成员变量"<<endl;

		Fun *f1 = (Fun*) p[0];
		(*f1[0])(); //? 这里为何A:mptr没有被赋值
//		(*f1[1])();
		cout<<"虚拟函数表：\n  先存放父类函数成员，\
			\n  覆盖就是替换了VF表中父类函数成员入口"<<endl;

	}

	if(1){
		const int  int1=0 ;
		/*
			 if const   1. copy constructor
			            2. default constructor
		const B  b1 = 10 ;
		const B  b2= b1;
		printf(" type size of B is %d\n", sizeof(B));
		*/
		B b1 ;
		const B B2; //没有构造函数就会报错 comple error
		const B b2 = b1;
//		b2.f();
		// const 类型的类也不能访问它的虚函数
		cout<<"sizeof(B)="<<sizeof(B)<<endl;	
//		printf(" const has mptr=%d\n", b1.mptr);
		D *d,d4;
		A a,*a4;
		B *b,b4;
		d = reinterpret_cast<D*>(b);
// 		d = static_cast<D*>(b);   // compile error
//		d4 = dynamic_cast<D>(b4); // compile error,  type-id 必须是pointer/references
		// d = dynamic_cast<D*>(b);  segment fault
		int *i1=new int(3);
//		int *i2= 3; //这样不可以：
		d = reinterpret_cast<D*>(i1);
		const  int  i2 = 4;
		int &i3=const_cast<int&>(i2); //ref
		i3=8;
		int *i4=const_cast<int*>(i1); //pointer
		printf("%d\n", i3);
		printf("%d\n", *i4);

		//i2=5;

			
	}
	return 0;
}
