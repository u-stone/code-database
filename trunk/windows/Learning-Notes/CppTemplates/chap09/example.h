#ifndef _EXAMPLE_H
#define _EXAMPLE_H

#include <iostream>
using namespace  std;

/*ģ���е�����*/

//���Ʋ���
//����ʽѧϰ֮ǰ�ȿ��������ʽ��ͣ����������õ�������Ի��ˣ����Է����������
//����id(qulified-id)�����������������ռ���Ϊǰ׺�ı�ʾ����������ͨ��ʾ���������id(+ - * new [] �ȿ���λ��operator֮��������ص��������)������ת��id������������ʽ����ת�������������ģ���ʾ��
//��������(qulified name)��������idǰ����ϳ�Ա���ʷ���.����->���ķ�����id�����磺S::x��this->f,p->A::m�����������Ա���ʷ�������ʽ�����ģ��������������ĵģ��ǾͲ������������ƣ�����ǰ��˵��this->fд��f���Ͳ�����������
//����������(dependent name)��һ������ĳ�ַ�ʽ��������ģ����������ơ�����:
//                          1. ��ʽ����ģ�����T�����޺ͷ��������ƣ�
//                          2. �ó�Ա���������(.����->)�޶����������ƣ��������������ߵı��ʽ������������ģ�����T�ģ�
//                          3. ģ���г��ֵ�this->b�е�b��
//                          4. ��������ident(x, y, z)����ĳ��������������ģ��Ĳ�������ôidentҲ�����������ƣ��е����ǡ�������

//�ȿ���ģ���
int x;
class B{
public:
	int i;
};
class D : public B{
};

void testT1(D* pd){
	if (pd)
		pd->i = 3;   //��ȷ
	//D::x = 2;  //���󣬺���Ȼ�����Ƕ�֪��D������û��x�ģ�����ȫ��������������x��Ҳ�ǲ����Եģ�����error C2039: ��x��: ���ǡ�D���ĳ�Ա
	//���������ǵõ��Ĺ����ǣ�������������Ʋ��Ұ�������࣬���ǲ�������̳���ϵ�ܵ���Χ����������ע��������������������Ʋ���
}

extern int count;              //(1)
int lookup_example(int count){ //(2)
	if (count < 0){
		int count = 1;         //(3)
		lookup_example(count);//�����޵�count�����������(3)
	}
	return count +            //�����޵�count���������õ���(2)
		::count;              //���޵�count���������õ���(1)
}
//�������ݿ�������ǰ�򵥡�ֱ�۵�֪ʶ���������濴C++����ģ��֮�󣬷��������Ʒ�����ʲô

template <typename T>
inline T const& max__(T const& a, T const& b){
	return a < b ? b : a;
}

namespace BigMath{
	class BigNumber{

	};
	bool operator < (BigNumber const&, BigNumber const&){
		return false;
	}
}

using BigMath::BigNumber;

void g(BigNumber const& a, BigNumber const& b){
	BigNumber x = max__(a, b);//max__ʵ������ʱ���ǲ�֪��BigNumber�Ķ���ģ��������������ȷִ�У�
	                          //��ΪC++����һ���������Argument Dependent Lookup��ADL�������ڲ����Ĳ���
	                          //Ҳ����ADL�����ԭ��������˳�����Ʋ���
}

//���濴ADL��ϸ��

//ADL����Ҳ�����ص�namespace����class��ص�������
template <typename T>
class C{
	friend void f();
	friend void f(C<T> const &);//��ȷ��gcc����ʾ�����棺��Ԫ������void f(const C<T>&)��������һ����ģ�庯��
};

void testT2(C<int>* p){
	//f();    //����f()û�в����ͷ���ֵ���͵Ĺ�������߹��������ռ䣬����������������ҵ���C�У��������һ����Ч�ĺ������á�����error C2780: ��void f(const C<T> &)��: Ӧ���� 1 ��������ȴ�ṩ�� 0 ��
	f(*p);  //����ADLԭ����Ϊf(*p)�еĲ�������������C�������Ϳ�������C���ҵ���Ԫ�������������ҵ������֣��Ϳ���ͨ�������ˡ�
}
//��C����Ԫ������һ���ػ�
void f(C<int> const&){
}

void f(){//ע������testT2()�е�f()�Ҳ������������ǰ���ˣ���ô�ǿ����ҵ��ģ��������Ļ�testT2()�е�f()�͸���C�е���Ԫ����ûʲô��ϵ��

}

//����������������ʽ�����ơ�������������������в����˸�������ƣ���Ӧ��ģ��Ҳ�����Ƶ����

//�ȿ���ģ����Ĳ���ʽ������
int E;

class E{
private:
	int i[2];
public:
	static int fsize(){
		return sizeof(E);//�в���ʽ������E
	}
};

int fsize(){
	return sizeof(E);
}

void testT3(){
	cout << "E::fsize() = " << E::fsize() << ", "//����E���õ�������E��sizeof
		<< " ::fsize() = " << ::fsize() << endl; //����E���õ���int��sizeof
	//��������E::fsize() = 8,  ::fsize() = 4
}

//�ٿ�ģ�����͵Ĳ���ʽ������
template <template <typename T> class TT>
class X{

};

template <typename T>
class CY{
	CY* a;        //������CY������Ȼû��ģ�����������Ҳ�ᱻ����CY<T>* a;
	CY<void> b;   //������CY������ģ��ʵ�Σ��Ǿ��ǲ���ʽ��ģ������
	//���϶��߶��ǲ���ʽ��ģ������

	//X<CY> c;      //����CY����û��ģ��������ǲ��ᱻ����ģ�����Ƶģ�����:error C3200: ��CY<T>��: ģ�������TT����ģ�������Ч��Ӧ������ģ��
	X<::CY> d;    //��ȷ��CYǰ������������÷�::��Ϊ�������ƣ��ᱻ��Ϊ��ģ�����Ƶģ������ϵı������������ͨ��������ģ���Ϊ<:����[�������µı���������ʶ����
	X< ::CY> e;   //����Ϊ�˼����ϵı�������д��
};

//////////////////////////////////////////////////////////////////////////
//���濴һ�������������ƣ����������C++��Ϊһ��������������Զ�����
//�ٸ����ӣ�X<1>(0)�ĺ����ȡ����X�����Ǹ�ɶ�������һ��ģ��Ҳ���ԣ���һ������Ҳ���ԣ�����Ϊ(x<1)>0
//��ô��ģ���У����ǿ�һ������ģ�����ʽ�ػ�����ɵ������ж�����
template <typename T>
class Trap{
public:
	enum { x };
};
template <typename T>
class Victim{
public:
	int y;
	void poof(){
		Trap<T>::x*y;//VS2010�а������Ϊ��Trap<T>::x���͵�ָ�룬��û�а������Ϊ��Trap<T>::x��y�Ļ�
		y = NULL;
	}
};
template<>
class Trap<void>{
public:
	typedef int x;
};
void boom(Victim<void>& bomb){
	bomb.poof();
}
template <typename T>
class XX{
	//template <typename T>
	class Base{

	};
};
//C++���Թ涨(�����б仯)��ͨ�����ԣ��������������Ʋ��������һ�����ͣ��������������֮ǰ�йؼ���typename��
//����Ĺ����ǣ���������������ʱ������ʹ��typename��ʹ��class�ǲ����Եģ�
//(a)���Ƴ�����ģ���У�����˵�ڷ�ģ�嶨���г��֣��ǻᱨ��ģ��൱�ڷϻ���
//(b)���������޵�(�ο���ǰ�������)
//(c)���Ʋ�������ָ������̳��б��У�Ҳ����λ�ڹ��캯���ĳ�Ա��ʼ���б���λ��趨������������˵��ǰ��һ�϶�һ�񶨣���������ͷ�Ρ�ע���������˼������ڼ̳��б�ͳ�ʼ���б��в���ʹ��typename
//(d)����������ģ�����
//(e)ֻ����(a)��(b)��(c)��������ͬʱ�����ʱ�����ʹ��typename
template <typename T>               //(1) ����������ģ�����ǰ׺
struct S : /*typename*/ XX<T>::Base{//(2) ����Υ������(c)������error C2518: �ؼ��֡�typename���ڻ����б��зǷ��������ԣ���gcc�б�������expected class-name before ��XX��

	S() : /*typename*/ XX<T>::Base( //(3) ����������һ�£���û��typename���ǿ��Եģ������ǹ�����˵�Ĳ�����typename
		/*typename*/ XX<T>::Base(0))//(4) ͬ�ϣ��������C++�Ĺ���(c)�б仯��
	{
	}
	typename XX<T> f(){             //(5) ��û��typename�����ԣ�
		typename XX<T>::C *p;       //(6) ��û��typename�����ԣ�
		X<T>::D* q;                 
	}
	typename XX<int>::C * s;        //(7) ��û��typename������
};

struct U{
	//typename XX<int>::C * pc;     //(8) ����Υ������(a)������  error C2899: ������ģ������֮��ʹ���������ƣ�
};

void testTII(){
	//S<int> s;        //����������д��룬��ô����ܶ��д��붼�ᱨ��������Ҫ˵��ʵ���������кܶ���벢û��ʵ���������Ǳ����������ӳٱ����ƹ���
}

//���渽һ��gcc�еı�����Ϣ
template <typename T>                //(1) ����������ģ�����ǰ׺
struct S2 : /*typename*/ XX<T>::Base{//(2) ��������expected class-name before ��XX��
	S2() : /*typename*/ XX<T>::Base( //(3) ���󣺹ؼ��֡�typename�����������ڴ���������(�޶��ĳ�ʼֵ�趨��ʽ����һ������)
		/*typename*/ XX<T>::Base(0)){//(4) 
	}
	/*typename*/ XX<T> f(){         //(5) ��������expected nested-name-specifier
		typename XX<T>::C *p;       //(6) ��ȷ
		//X<T>::D* q;               // �������󣺡�q���ڴ�����������δ���������ǰ�����typenameǰ׺�Ϳ�����
	}
	typename XX<int>::C * s;        //(7) ��û��typename������
};

struct U2{
	//typename XX<int>::C * pc;     //(8) ����  ������ģ����ʹ�á�typename��  ������Υ���˹���(a)
};

//////////////////////////////////////////////////////////////////////////
//������ģ������ 
template <typename T>
class Shell{
public:
	template <int N>
	class In{//Ƕ����
	public:
		template <int m>
		class Deep{//Ƕ�����е�Ƕ����
		public:
			virtual void f();
		};
	};
};

template <typename T, int N>
class Weird{
public:
	//����ʹ����Ƕ������������������˵�¶�
	void case1(typename Shell<T>::template In<N>::template Deep<N>* p){//��ȷ
		p->template Deep<N>::f();//ע�������麯��
	}
	void case2(typename Shell<T>::template In<N>::template Deep<N>& p){//��ȷ
		p.template Deep<N>::f();
	}
	void case3(typename Shell<T>::In<N>::Deep<N>* p){//��ȷ������template�ؼ���Ҳ���ԣ��ɵı������Ͽ��ܲ���
		p->Deep<N>::f();
	}
	//void case4(Shell<T>::In<N>::Deep<N>* p){//���󣬲���typename�ǲ��Ϸ��ģ�����error C2061: �﷨����: ��ʶ����Deep��
	// p->Deep<N>::f();
	//}
	//void case5(Shell<T>::In<N>::Deep<N>& p){//���󣬲���typename�ǲ��Ϸ��ģ�����error C2061: �﷨����: ��ʶ����Deep��
	// p.template Deep<N>::f();
	//}
	//ָ���������Ǻ��������������ͬ��
};
//����
//Ҳ����˵�����Ҫ��һ�������Ƕ����˵�����ͣ���ô����ʡȥtemplate�ؼ��֣�������ǰ��������typename�ؼ��֣���Ӧ�����µ�C++��������ģ���֧��
//��һ���ϣ��ھɵı�������template�ǲ�����ʡ�Եġ�

//���ſ�ʹ����using֮���Ч��

//ʹ��using����������������ռ������
class BX{
public:
	void f(int);
	void f(char const *);
	void g();
};
class DX : private BX{
public:
	using BX::f;//������DX�оͿ���ʹ��BX�е�f�ˣ����ﲢû��ָ������һ��f����������д���ƻ���C++�ķ��ʼ����������������ܲ�������������
	//BX::f;  //���ֻ��ƿ��ܻ�ȡ������ģ�����ֻ��ʹ��һ��
};
//������������д������˵����������͵����������ơ�ģ�����ơ�������������
template <typename T>
class BXT{
public:
	typedef T Mystery;   //����һ������
	template <typename U>
	struct Magic;
};

//��������
template <typename T>
class DXTT : private BXT<T>{
public:
	using typename BXT<T>::Mystery;//�л���û�����У����涼����ȷ��
	Mystery * p;                    //��ȷ
};

//����ģ����
template <typename T>
class DXTM : private BXT<T> {
public:
	using BXT<T>::/*template*/ Magic;//�л���û�����У����涼����ȷ��
	Magic<T>* plink;                   //��ȷ
};

//ADL����ʽģ��ʵ��
namespace N{
	class X{
	};
	template <int I>
	void select(X* );
};
void gnx(N::X* xp){
	using namespace N;  //���û���������������ռ�N������ľ��Ǵ�ģ�����error C2065: ��select��: δ�����ı�ʶ��������������У�����ľͿ����ҵ�select()��������˵����һ�в����ܸ���ADL�ҵ�select��ʶ�����������ƽ�е�
	//N::select;     //���������ǲ��еģ��������using��Ч�����ǲ�һ��
	select<3>(xp);   //ȡ����using namespace N;һ��
}
void N::select<3>(X* ){//select�Ķ���

}

//���������ģ��
template <typename X>
class Base{
public:
	int basefield;             //ע�⣬basefield��һ��������������
	typedef int T;             //�����ͺ��ĵ��÷�һ�����һ����ֱ�۵�����
};

class D1 : public Base<Base<void>>{//��ģ�壬����Ļ��������Ƿ��������ƣ���������ȫȷ���ģ���������ģ�����
public:
	void f(){
		basefield = 3;         //ע�⣬basefield��һ��������������
	}
};
template <typename T>
class D2 : public Base<double>{
public:
	void f(){
		basefield = 7;
	}
	T strange;                 //ע�������T����ģ������е�T�����Ƿ��������ͻ����е�typedef֮���T����һ���ǲ�ֱ�۵�
};

void gD2(D2<int*>& d2, int* p){
	//d2.strange = p;           //���󣬱���error C2440: ��=��: �޷��ӡ�int *��ת��Ϊ��Base<X>::T��
	              //�����Ҫ��ϸ����d2.strange����ΪBase<double>::T���ͣ��������������int������typdef�Ǿ䶨���T����������D2�����е�ģ���������(�����������int*)��
	              //��Ϊ���ڻ���(�����Base<Base<void>>)�Ƿ��������͵�ʱ�������������в���һ�����������ƣ����Ȳ���������࣬Ȼ�����ģ��������
}

//C++�й涨������ģ���еķ����������ƣ������ڿ����ĵ�һʱ��������Ʋ��ң���������ʵ������ʱ��Ų���
//           ���ڷ������������Ʋ������������ͻ����в��ҡ�
//     ���ǣ����������������ƣ�ֻ����ʵ������ʱ��Ż���в��ҡ���ʱ�򣬻���Ҳ�����ػ����ˣ������������͡�
template <typename T>
class DD : public Base<T>{//���Base<T>�����������ͻ�����
public:
	void f(){
		//basefield = 0;//(position 1)������ԭ������ģ�����
		/*
		error C2440: ��=��: �޷��ӡ�int��ת��Ϊ����
		ת��Ϊö������Ҫ����ʽת��(static_cast��C ��ʽת��������ʽת��)
		g:\visual studio 2010\projects\cpptemplates\cpptemplates\chap09\example.h(326): ������ ģ�� ��Ա������void DD<T>::f(void)��ʱ
		with
		[
		T=bool
		]
		g:\visual studio 2010\projects\cpptemplates\cpptemplates\chap09\example.h(335): �μ������ڱ������ ģ�� ʵ������DD<T>��������
		with
		[
		T=bool
		]
		*/
	}
};
template <>
class Base<bool>{
public:
	enum {basefield = 42};//(position 2)
};

void gDD(DD<bool>& d){
	d.f();//(position 3) ����Ĵ�������������
}
//����ģ�position 1����������������basefield��һ�����������ƣ���ô�����������Ʋ��ң�����Base���ҵ�����int�ͱ��������û��(position 3)��������û�������
//�����ڣ�position 3������d��Ϊһ����ʽʵ�����Ķ��󣬻�����Base<bool>�ػ����壬�����ֽ�basefield����Ϊ��enum���ͳ�������˾������汨��˵�޷���������ת��
//Ϊ�˽����������⣬���ǿ�����f()�����е�basefield������������ƣ������Ϳ��Խ����ӳٲ����ˣ�����д����
//                ��1��this->basefield = 0;     ����
//                ��2��ͨ�������������������ͣ�Base<T>::basefield = 0;
//������1��
template <typename T>
class DD1 : public Base<T>{
public:
	void f(){
		this->basefield = 0;
	}
};
//������2��
template <typename T>
class DD2 : public Base <T> {
public:
	void f(){
		Base<T>::basefield = 0;
	}
};

//������һ�����Σ�˵����һ�ַ����ȵڶ��ַ�������(�������µı������Ϻ�����)
template <typename T>
class BT{
public:
	enum E{e1 = 6, e2 = 8, e3 = 90};
	virtual void zero(E e = e1){std::cout << "BT<T>::zero()" << std::endl;};
	virtual void one(E&){std::cout << "BT<T>::one()" << endl;}
};

template <typename T>
class DT : public BT<T>{
public:
	void f(){
		typename DT<T>::E e;
		this->zero();
		one(e);
	}
	virtual void zero(E e = e1){
		std::cout << "DT<T>::zero()" << std::endl;
	}
	virtual void one(E&){
		std::cout << "DT<T>::one()" << std::endl;
	}
};
template <typename T>
class DDT : public DT<T>{
	virtual void zero(E e = e1){
		std::cout << "DDT<T>::zero()" << std::endl;
	}
	virtual void one(E&){
		std::cout << "DDT<T>::one()" << std::endl;
	}

};

void testT4(){
	DT<int> *pddt = new DDT<int>;
	pddt->f();//��ȷ�������
	/*
	DDT<T>::zero()
	DDT<T>::one()
	*/
	delete pddt;
	DT<int> * pdt = new DT<int>;
	pdt->f();//��ȷ�������
	/*
	DT<T>::zero()
	DT<T>::one()
	*/
	delete pdt;
}
//������﷨û�����⣬���Ҳû�����⣬�����µ�C++��������û�н���zero()��one()���麯�����ã������ھɵı�������f()�ĵ���ʱ�ᱨ��ģ�
//////////////////////////////////////////////////////////////////////////

#endif