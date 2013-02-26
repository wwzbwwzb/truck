
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
