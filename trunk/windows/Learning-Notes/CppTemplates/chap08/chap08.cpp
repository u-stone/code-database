// chap08.cpp : �������̨Ӧ�ó������ڵ㡣
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
	//����û�е��õ�ģ�庯������ʹ���������Ҳ��������ͨ������

	//ģ����
	List<int> lst;
	List<int> lst2 = lst;//ģ�����͵��ⲽ�����ƺ�����ͨ�಻ͬ��lst2û�е����κι��캯�����������캯�������Ǹ�ֵ���캯��
	lst2 = lst;
	List<double> lstd = lst;//��һ������֮�󣬾ͺ���ͨ��һ���ˡ�
	lstd = lst;
	//��ͨ��
	Test t1;
	Test t2 = t1;
	t2 = t1;
	//output��
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
