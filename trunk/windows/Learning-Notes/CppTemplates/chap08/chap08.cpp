// chap08.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "example1.h"
#include "Stack.h"

class Test{
public:
	Test(){
		cout << "Test Constructor" << endl;
	}
	Test(const Test& ){
		cout << "Test Copy Constructor" << endl;
	}
	Test& operator = (const Test& ){
		cout << "Test assingment constructor" << endl;
		return *this;
	}
};


template <typename T>
void external();


int _tmain(int argc, _TCHAR* argv[])
{
	//对于没有调用的模板函数，即使定义错误了也照样可以通过编译

	//模板类
	List<int> lst;
	List<int> lst2 = lst;//模板类型的这步操作似乎和普通类不同，lst2没有调用任何构造函数、拷贝构造函数或者是赋值构造函数
	lst2 = lst;
	List<double> lstd = lst;//换一个类型之后，就和普通类一样了。
	lstd = lst;
	//普通类
	Test t1;
	Test t2 = t1;
	t2 = t1;
	//output：
	/* 
		List Constructor
		List Copy Constructor
		List Assignment Constructor
		Test Constructor
		Test Copy Constructor
		Test assingment constructor
	*/

	fill(&Array<int>());//output : report_top() function

	//=========================================================================
	external<int>();// output: external called external() function

	//=========================================================================
	testT1();
	testT2();
	testT13();
	return 0;
}
