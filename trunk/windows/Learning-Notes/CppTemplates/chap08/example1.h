#ifndef _EXAMPLE01_H
#define _EXAMPLE01_H

#include "Stack.h"
#include <iostream>
using namespace std;

//=========================================================================
template <typename T>
class List{
public:
	//template <typename T>
	List(){
		cout << "List Constructor" << endl;
	}
	template <typename T2>
	List(const List<T2>& );

	template <typename T2>
	List& operator = (const List<T2>&){
		cout << "List Assignment Constructor" << endl;
		return *this;
	}
};

//ע�������˳������Ǵ���Χ���ڲ��������ͨ��������ġ�
//������List<T>::{ctor}��: �޷����������������е�����ƥ��
template <typename T>
template <typename T2>
List<T>::List(List<T2> const &){//λ�����ⲿ�ĺ�������
	cout << "List Copy Constructor" << endl;
}

template <typename T>
int Length(List<T> const &);

//=========================================================================
class Collection{//��Χ��
	//�ڲ���2�� Node�� Handle
	template <typename T>
	class Node{
		
	};

	template <typename T>
	class Handle;

	template <typename T>
	T* alloc(){
		
	}
};

template <typename T>
class Collection::Handle{//�ڲ�����ⲿ���壨��Ҫ�����޶�����
	
};


//=========================================================================
//���ϵ�ģ��

template <typename T>
union AllocChunk{
	T object;
	unsigned char bytes[sizeof(T)];
};

//=========================================================================
//ģ�������ʹ��ȱʡֵ
template <typename T>
void report_top(Stack<T> const& , int number = 100){
	cout << "report_top() function" << endl;
}

template <typename T>
class Array{

};

template <typename T>
void fill(Array<T>* , T const& = T()){
	Stack<int> ary;
	report_top(ary);
}

//=========================================================================
//���Ա�������ܶ���Ϊģ�壬��Ϊ�麯��������Ҫ֪����Ա�����ĸ�����������Ҫ�ȵ�ʵ���������֪����

class Dynamic{
public:
	virtual ~Dynamic(){}

// 	template <typename T>
// 	virtual void copy(T);//����д��󣬱���ע�͵�
};

//=========================================================================
//ģ�������

//���������ֵ�����
int C;
class C;  //�����ͷ����ͱ��������ǿ��Եģ�����������������ͬһ�����ֿռ�

int X;
template <typename T>
class X;   //����ģ����˵���������������������������汾�ı������Ͽ��ܲ��У�����������ΪMicrosoft (R) Message Compiler  Version 1.12.7600��
           /*
		   (GCC) 4.4.7 20120313 (Red Hat 4.4.7-3)�ϱ���  ���󣺡�template<class T> struct X������������Ϊ��ͬ����ķ���
		   */

//struct S;  //��������г�ͻ������ע�͵�
template <typename T>
class S;//������S��: �� ģ�� �Ѿ�����Ϊ���� ģ��

//�ٴ������ֵ����ӷ�ʽ��ģ�岻������C����
extern "C++"   //����Ĭ�ϵķ�ʽ����дҲ����
	template <typename T>
void normal();

//extern "C"   //���� error C2894: ģ�岻������Ϊ�С�C������
	template <typename T>
void invalid();

//extern "Xroma" //�еı���������֧�֣���VC2010��֧�֣����� error C2537: ��Xroma��: �Ƿ������ӹ淶
	template <typename T>
void Xroma_link();

//����ǹ�����������

template <typename T>//�����������ֱ���ڱ���ļ��е��ã�������ͨ�������÷�
void external(){
	cout << "external called external() function" << endl;
}

template <typename T>//��������Ͳ����ڱ���ļ��е��ã�static������Ϊ���ļ��ڵġ���ô����ļ��оͿ�����ͬ���ĺ���
static void internal();

//ģ�庯���������������ǻ���������Ҳ���ǻ���ģ�壬�������ں������������һ�Լ�����
template <typename T>
static void basedeclare();

//�Ǿ�̸һ��ģ��ľֲ��ػ��������������ڻ���ģ��
template <typename T1, typename T2>
class MyClass{
	//����ģ��
};

//����ȫ���Ǿֲ��ػ�
template <typename T>
class MyClass<T, T>{
	//����ģ�����������ͬ���͵�
};

template <typename T>
class MyClass<T, int>{
	//�ڶ�������������int
};

template <typename T1, typename T2>
class MyClass<T1*, T2*>{
	//ָ�����͵�ģ�����
};

//������˵��:
template <>
class MyClass<int, int>{//������ģ���һ���ػ�

};
template <typename T>
class MyClass<T*, T*>{

};

MyClass<int, float> mif;      //MyClass<T1, T2>
MyClass<float, float> miff;   //MyClass<T, T>
MyClass<float, int> mfi;      //MyClass<T, int>
MyClass<int*, float*> mp;     //MyClass<T1*, T2*>

//�ֲ��ػ��Ķ�����    �����һ���TMD���׳������
MyClass<int, int>   m1; //ͬ�ȳ̶�ƥ��MyClass<T, T> & MyClass<T, int>
MyClass<int*, int*> m2; //ͬ�ȳ̶�ƥ��MyClass<T, T>  & MyClass<T1*, T2*>
//�������ͨ������µ�������������Ѿ������������
//template <>
//class MyClass<int, int>{
//
//};
// template <typename T>
// class MyClass<T*, T*>{
// 	
// };

//=========================================================================
//ģ����������ͣ������ͣ�ģ������

//ģ������е����;��൱����typedef֮������ͣ��������������в������ظ������������
template <typename Allocator>
class List_{
	//class Allocator* allocator;//����:��typename��: ���ǡ�class��
	//friend class Allocator;    //����: ��typename��: ���ǡ�class��
};

//�����Ͳ���, ����˵�����ڻ������ڿ���ȷ���ĳ�ֵ:���ͻ�ö�����ͣ�ָ�����ͣ��������͡�
//�������ֻ������ֵ�����ܱ�ȡ��ַ���߸�ֵ
template <int buf[5]> class Lexer;
template <int *buf> class Lexer;    //���������ͬ��
//��Щ�������ܱ�static mutable���Σ�ֻ�ܱ�const��volatile����

//ģ��������Ϊģ�����
template <
	template <typename T> class C
>
	void f1(C<int>* p);

//���ǲ�����ʹ��struct��union
//template <
//	template <typename T> struct C//���� error C2144: �﷨����:��void����ǰ��Ӧ�С�;��
//>
//	void f2(C<int>* p);
//
//template <
//	template <typename T> union C//����ͬ��
//>
//	void f3(C<int>* p);

class MyAllocator;

template <
	template <typename T, typename A = MyAllocator> class Container
>
class Adaptation{
	Container<int> storage; //ʹ����ȱʡ�Ĳ���Allocator
};

//template <
//	template <typename T, T*> class Buf
//>
//class Lexer{
//	static char storage[5];
//	Buf<char, &(Lexer<Buf>::storage[0])> buf;//�����VC2010�ϱ��벻ͨ��
//};

template <
	template <typename T> class List
>
class Node{
	struct T storage;//����ʹ����ģ���ģ�����T���������еı�������T�ǲ�����ʹ�ã�����Ϊ����ʹ��
};

//����ģ���ģ������Ĳ�������������һ�㲻��ʹ�ã��ʶ�����ʡ��
template <
	template <typename /*T*/, typename /*allocator*/ = MyAllocator>//ʡ��������
		class Container
>
class Adaptation_{
	Container<int> storage;//
};

//=========================================================================
//ȱʡģ��������κ����͵�ģ�������������ȱʡ����

template <typename T1, typename T2, typename T3, typename T4 = char, typename T5 = char>
class Quintuple;

template <typename T1, typename T2, typename T3 = int, typename T4, typename T5>//���������T4 T5��ȱʡ����
class Quintuple;

//template <typename T1 = char, typename T2, typename T3, typename T4, typename T5>//���󣬱���error C3747: ȱ��Ĭ�� ģ�� ����: ���� 2
//class Quintuple;

//ȱʡ�����ǲ������ظ���
template <typename T = void>
class Value;

//template <typename T = void>//�����棺 warning C4348: ��Value��: �ض���Ĭ�ϲ��� : ���� 1
//class Value;

void cmp(int a, double b = 1.0);
void cmp(int a = 1, double b);//��ͨ�ĺ�������Ҳ���Խ���ͬ��������ȱʡ������ͬģ�������ͬ����ʵ�ϣ������b��������д��double b = 1.0�����򱨴� error C2572: ��cmp��: �ض���Ĭ�ϲ��� : ���� 2

//=========================================================================
//ģ��ʵ��:����ģ��ʵ������ʱ�������滻ģ�������ֵ
//��Ϊ�������ͣ�1 ��ʾģ�������2 ע��ʽ�����ƣ�3 ȱʡģ�������4 ʵ������

//��ʾģ�����
template <typename T>
inline T const& max_(T const&a, T const &b){
	return a < b ? b : a;
}
double normal(double a = 1.02){
	return a;
}

void testT1(){//�Ƶ�main�����в���
	cout << max_<double>(1.0, -3.0) << endl;//��ʽ1����ʾָ�� , output: 1
	//?_?
	cout << normal() << endl;
	double tmp;
	cout << (tmp = max_(2.0, -3.0)) << endl;//��ʽ4��ʵ������Ϊdouble  , output: 1
	//cout << max_<int>(2.0, 3.0) << endl;    //��ʽ1����ʾָ��int��ֹ��������ص���int���� , output: 3 ���棺warning C4244: ��������: �ӡ�double��ת������const int�������ܶ�ʧ����
}

template <typename DstT, typename SrcT>//������ģ��ʵ�������Ҫ����ģ������б��ǰ�棬�����ķź���׼������ʵ������
inline DstT implicit_cast(SrcT const& x){//SrcT����ͨ��ʵ�����DstT����ͨ����ʾģ������ķ�ʽָ��
	return x;
}

template <typename SrcT, typename DstT>//�����������ӣ������л���˳��Ӱ�������ʹ�á�
inline DstT implicit_cast_2(SrcT const& x){
	return x;
}

void testT2(){
	cout << implicit_cast<double>(-1) << endl;//����������У�DstT����ʾָ��Ϊdouble����SrcTͨ��ʵ������Ϊ��int��Ҳ����˵ͬʱ��������ʾģ��ʵ�κ�ʵ������
	//cout << implicit_cast_2<double>(-1) << endl;//���� error C2783: ��DstT implicit_cast_2(const SrcT &)��: δ��Ϊ��DstT���Ƶ� ģ�� ����
	cout << implicit_cast_2<int, double>(-1) << endl;//����ģ��ʵ�ζ�ʹ������ʾģ��ʵ�εķ�ʽָ��
}

//���ں���ģ������ĵ���
template <typename Func, typename T>
void apply(Func func_ptr, T x){
	func_ptr(x);
}
template <typename T> void single(T){}

template <typename T> void multi(T){}
template <typename T> void multi(T*){}

void testT3(){
	apply(&single<int>, 3);
	//apply(&multi<int>, 7);//����multi���ڶ����ԣ�����error C2914: ��apply��: ��Ϊ������������ȷ�������޷��Ƶ��� ģ�� ����
}

//����ģ���С����족��SFINAE��substitution-failure-is-not-an-error�����滻ʧ�ܲ��Ǵ���
typedef char RT1;
typedef struct {
	char a[2];
}RT2;
RT2	 rt2;

template <typename T> RT1 test(typename T::X const* ){}//���T��û��X�����ͣ���ô��ͼ�������������ͻ��ǲ��ᱨ���
template <typename T> RT2 test(...){ return rt2;}


void testT4(){
	test<int>();//���õ���test(...)����һ��test��������ʧ�ܣ�
}

//�������Ҫ��ͼ���滻ʧ�ܵ����ͽ��м��㣬�ǾͲ������ˣ����������SFINAE�������������$chap15 & chap19$���и�������
// template <int I> void f(int (&)[24/(4-I)]);
// template <int I> void f(int (&)[24/(4+I)]);
// 
// void testT5(){//�������������VC2010 gcc4.4.7�϶���֧��
// 	&f<4>;//�滻���һ������Ϊ0
// }

//���ڲ�����Ϊģ��ʵ�ε����ͣ�1. δ�������ࡢö������(�ⲻ�Ƿϻ����û�����֣�����ô�����أ�)�����ǿ���ͨ��typedef���ı���һ�㣬���ǲ�ͬ�ı�����֧�̶ֳȲ�һ����2. �ڲ��� ���ڲ�ö��
class {//������
	int a;
};

typedef struct  
{
	double x, y, z;
}Point;

typedef enum {red, green, blue} *ColorPtr;

void testT6(){
	struct Association{
		int *p;
		int *q;
	};
	List<Association*> error1; //vc2010��û���⣬gcc�ϱ������󣺡�template<class T> class List����ģ��ʵ��ʹ���˾ֲ����͡�test()::Association*��
	List<ColorPtr> error2;//VC2010��û���⣬gcc�ϱ������󣺡�<anonymous enum>*����/ʹ������������
	List<Point> ok;
}

//���о��ǣ��滻֮�����֧����Ӧ�Ĳ����ſ���
template <typename T>
void clear(T p){
	//*p = 0;//������˵��
}

void testT7(){
	int a = 0;
	clear(a);//����error C2100: �Ƿ��ļ��Ѱַ
}

//�������漸�ַ����͵�ģ��ʵ��
template <typename T, T nontype_param>
class Class;

Class<int, 33>* c1;   //�����ڿ���ȷ�������ͳ���������˵�������ͳ����ǲ�������ģ�������
//Class<double, 33.33> * cd;//���� ���� error C2993: ��double��: ������ģ�������nontype_param�������ͷǷ�

int a;
Class<int*, &a>* c2;    //�ⲿ�����ĵ�ַ

void testT8(){
	int a;
	//Class<int*, &a>* c2;    //���ⲿ�����ĵ�ַ������
}

void f();
void f(int);
Class<void (*)(int), f>* c3; //�ⲿ����������ĵ�ַ���ò���������ʡ��

void testT9(){
	void f_inner(int);
	Class<void (*)(int), f_inner>* c3; //�ڲ�����������ĵ�ַ���ò���������ʡ��
}

int arr[5];
Class<int [5], arr>* c4;   //�����ⲿ������������˵�����ﱾӦ�����������aһ���ĵ�ַ���ò�����������Ҳ����ʡ��

void testT10(){
	int arr_inner[5];
	//Class<int [5], arr_inner>* c4;   //�ֲ����������� ���󣬱���error C2971: ��Class��: ģ�������nontype_param��:��arr_inner��: �ֲ������������������Ͳ���
}

class XClass{
public:
	int n;
	static bool b;
};
Class<bool&, XClass::b>* c5;   //����static���͵����Ա������ֱ��ʹ�������������
Class<int XClass::*, &XClass::n> * c6;//���Ƕ��ڷ�static���͵����Ա���ͱ���ʹ��������(�� ����::)����

template <typename T>
void temp_func(){}
Class<void (*)(), &temp_func<double> >* c7;//����ģ��ʵ����ͬʱҲ�Ǻ���

//��Ϊ���⣬��Щ�����ǲ�����Ϊģ��ʵ�εģ�
int *nulptr = nullptr;//��ָ��
int *const nulptr2 = nullptr;//��ָ��
//Class<int*, nulptr>* c8; //VS2010���ǲ����Եģ�����error C2975: ��nontype_param��:��Class����ģ�������Ч��ӦΪ����ʱ�������ʽ
Class<int*, nulptr2>* c08;//����ָ�� ��ȷ

float flt = 0.0f;
Class<int*, nullptr> * c9;//��ȷ
//Class<float, flt> * c10;//���������ͣ����� error C2993: ��float��: ������ģ�������nontype_param�������ͷǷ�
int inte = 0;
const int con_inte = 0;
//Class<int, inte> * c11;//VS2010�ϲ����ԣ� ����error C2975: ��nontype_param��:��Class����ģ�������Ч��ӦΪ����ʱ�������ʽ
Class<int, con_inte> * c12;//��ȷ


char const str[] = "failed string";
extern char const str2[] = "succeed string";
//Class<char const*, str> * c13;//VS2010�ϱ���error C2970: ��Class��: ģ�������nontype_param��:��str��: �漰�����ڲ����ӵĶ���ı��ʽ�������������Ͳ���
Class<char const*, str2> * c14;//�ɹ�������ʹ����extern������ʹ��str2�������ⲿ����

class Base{
public:
	int a;
}base;

class Derived{
}derived;

//Class<Base*, &derived>* err1;//���� ����error C2440: ��specialization��: �޷��ӡ�Derived *��ת��Ϊ��Base *��
//Class<int*, &base.a> * err2;//���� ����error C2975: ��nontype_param��:��Class����ģ�������Ч��ӦΪ����ʱ�������ʽ
//Class<int Base::*, &base.a> * err3;//���� ����error C2975: ��nontype_param��:��Class����ģ�������Ч��ӦΪ����ʱ�������ʽ
Class<int Base::*, &Base::a> * suc4;//���� ����error C2975: ��nontype_param��:��Class����ģ�������Ч��ӦΪ����ʱ�������ʽ

int arr_i[10] = {0,1,2,3,4,5,6,7,8,9};
//Class<int*, &arr_i[0]> c15;//����error C2975: ��nontype_param��:��Class����ģ�������Ч��ӦΪ����ʱ�������ʽ

//ģ���ģ�����
#include <list>

template <typename T1, typename T2, 
	template<typename> class Container//�������ģ���βα���ʹ��class������typename����ȻҲ������struct, union
>
class Relation1{
public:
private:
	Container<T1> dom1;
	Container<T2> dom2;
};

void testT11(){
	//Relation1<int, double, std::list> rel;//���󣬱���error C3201: ��ģ�塰std::list����ģ������б���ģ�������Container����ģ������б�ƥ��
	//��Ϊstd::listʵ��������������������һ��ȱʡ��allocator����ģ��ʵ�Σ�std::list�����뾫ȷƥ����ģ���βΣ�����β���û��ȱʡֵ��ʵ�β����Դ�ȱʡ��������һ����������ӿ�����
	//         ����β�����ȱʡ������ʵ���е�ȱʡ�����ǲ������ǵģ�����˵��ʹ���β��е�ȱʡ��������������ʵ����Ϊȱʡ��������ʲôֵ��������Ӱ���滻ʱ���βΣ������testT13()֤ʵ��һ��
}
//�޸ĵķ����������������ʾ��
template < typename T1,  typename T2, 
	template< typename T, typename = std::allocator<T> > 
    class Container
    >
class Relation2{//ģ����
public:
	Container<T1> dom1;
	Container<T2> dom2;
};


void testT12(){
	Relation2<int, double, std::list> rel2;//��ȷ
	//Relation2<int, double, TestList<int, double>> rel3;//���󣬱���error C3201: ��ģ�塰TestList����ģ������б���ģ�������Container����ģ������б�ƥ��
}

template < typename T1,  typename T2, 
	template< typename T, typename T3 = char> 
    class Container
>
class Relation3{//ģ����
public:
	Container<T1> dom1;
	Container<T2> dom2;
	int size(){
		cout << "get dom2 size" << endl;
		return dom2.size();
	}
};

template <typename T1, 
	typename T2 = int
    >
class TestList{//ģ����
public:
	int size(){
		cout << "get T2 size :" << sizeof(T2) << endl;
		return sizeof(T2);
	}
};

void testT13(){
	Relation3<int, double, TestList> rel;
	cout << rel.size() << endl;//��������1 (����Container���ڲ���ģ�����ȱʡֵchar��size��sizeof(char))��������rel��TestListĬ�ϲ���int�õ���sizeof(int)=4
}


//ģ��ʵ�εĵȼ���

template <typename T, int I>
class Mix{};

typedef int Int;

void testT14(){
	Mix<int, 3*3> * pMix1;
	Mix<Int, 4+5> * pMix2;//pMix1 pMix2����ͬ�����ͣ�����typedef����֮�������ʵ�ʱȽϵĻ���ԭ�������ͣ����ڷ����͵�����ʵ�αȽϵ��ǲ�����ֵ��
	pMix2 = pMix1;//���Mix�ĵڶ���ģ��������͸�Ϊdouble,float֮�࣬����ֱ������
}

void testT15(){
	Mix<float, 3.0f> * pMix3;
	Mix<float, 3.8f> * pMix4;
	Mix<float, 4.0f> * pMix5;
	pMix4 = pMix3;//��ȷ�������3.8��������ת��������3
	//pMix5 = pMix3;//���󣬱��� 
	/*
	error C2440: ��=��: �޷��ӡ�Mix<T,I> *��ת��Ϊ��Mix<T,I> *��
	with
	[
	    T=float,
	    I=3
	]
	and
	[
	    T=float,
	    I=4     //ע�⿴����
	]
	��ָ��������޹أ�ת��Ҫ�� reinterpret_cast��C ��ʽת��������ʽת��
	*/
}

//��Ԫ

//�ȿ���Ԫ��
template <typename T>
class TreeNode;

template <typename T>
class Tree{
	//friend class TreeNode;//��Ϊһ��ģ���࣬�Ͳ�����������ʽ�ˣ�����error C2990: ��TreeNode��: ���� ģ�� �Ѿ�����Ϊ�� ģ��
	friend class TreeNode<T>;//��ȷ
	//friend class TreeNode2<T>; //������ʱ��ģ�岻֪��TreeNode2��ʲô������ error C2059: �﷨����:��<��
	friend class unknownClass;//���ڷ�ģ�����ͣ�������Ȼû��unknowClass���������������﷨����ġ�
};

template <typename T>
class TreeNode2;

//��Ԫ����
void multiply(void*);   //��ͨ����

template <typename T>
void multiply(T);      //ģ�庯��

//��ͨ��
class Comrades{
	friend void multiply(int) {}//�Ϸ����������ǰû�м�::��˵���������һ������ģ���ʵ�����������ﻹ������������״������Ͷ���
	friend void multiply(void*) {}//�Ϸ������Ǻ��ⲿmultiply(void)������ͬ������Ķ���
	friend void ::multiply(void*);//�Ϸ����������ǰ������::��˵����������ǰ���һ����ͨ����������ģ�庯����ʵ����������ͨ���������ȼ��Ⱥ���ģ������ȼ��ߣ�������������ͨ����������
	friend void ::multiply(int); //�Ϸ���������ķ�����֪�������Ǻ���ģ��ʵ��������
	//friend void multiply<int>(int){} //������Ȼ����ǰû��::�����Ǻ�����м����ţ�˵������ģ�庯����Ҫƥ�������ģ�庯��������ģ����Ԫ�������������ﶨ�塣����error C3637: ��multiply��: ��Ԫ�������岻���Ǻ��� ģ�� ��ר�û�
	friend void ::multiply<double>(double);//�Ϸ������޵����ƻ����Ծ���һ�Լ����ţ�����������������ǿɼ���
	//friend void invisuable<double>(double);//�������ڸú������ɼ�����������֪������ʲô���͡����ڷ�ģ����Ԫ��ͷ�ģ�庯����˵���Բ���ǰ��������ֱ�����ã�����ģ�庯�������ԣ�����error C2143: �﷨���� : ȱ�١�;��(�ڡ�<����ǰ��)
	//friend void ::invisuable2<double>(double);//�Ƿ���ǰ���ּ����������÷���˵������Ҫ����ǰ����������߶��壬�������ڲ��ɼ�������error C2039: ��invisuable2��: ���ǡ�`global namespace'���ĳ�Ա
	friend void invisuable();//�Ϸ��ģ���Ϊ�ú���������ĳ���ⲿ������ĺ��������ã�������ͨ��������Ϊ��Ԫ��������Ҫǰ��������
	//friend void ::error(){}//����ͬ�ϣ�����error C2039: ��error��: ���ǡ�`global namespace'���ĳ�Ա

	//Ϊ�˼�����Լ�ס�������ޣ������á��ɶ��塣���������޺������Ͳ�������Ϊ��Ԫ������˳��������һ����ͨ�ⲿ��Ԫ�����Ķ��壩
};

template <typename T>
class TreeNode2{
	TreeNode2<T> * allocator();//ģ���Ա����
};

//ģ�����е���Ԫ
template <typename T>
class List_TreeNode{
	friend TreeNode2<T> * TreeNode2::allocator();//������Ԫģ���Ա����
};

//��ģ������������Ԫ�����Ķ����ʱ�򣬻ᷢ����

template <typename T>
class Creator{
	friend void appear(){
		//��ǰ�����ӽ��ģ�������Զ���appear��appearǰ��û�������÷�::��
	}
};
//�����ڶ�������Creatorģ������ʱ����Ȥ������ͷ����ˣ�
Creator<double> miracle;//���ԣ���ʱ���ģ�����ʵ������������һ��appear()
//Creator<int> oops;//������ʱ���ģ�����ʵ��������������һ��appear()�������������������������һ���ģ�����
/*
	error C2084: ������void Creator<T>::appear(void)����������
	with
	[
		T=double
	]
*/

//���Խ���취������Ԫ�����б��������ģ���ģ���������ô���Ƶ���ȷ���������ǣ�

template <typename T>
class Creator2{
	friend void appear(T){//�����ͻ����ɲ�ͬ�ĺ���ģ��ʵ��
	}
};

Creator2<double> rightCreator;//����
Creator2<int> alsoRightCreator;//����

//�������Ԫ�ࡢ��Ԫ��������Ϊģ�壬Ҳ������Ԫģ�壬��ô��û���ģ����Ӧ�����еľֲ��ػ�����ʾ�ػ������������������Ԫ

template <typename T>
class Task;

template <typename T>
class Schedule{
	static void dispatch(Task<T>*);
};

class Manager{
	template <typename T>
	friend class Task;

	template <typename T>
	friend void Schedule<T>::dispatch(Task<T>*);

	template <typename T>
	friend int ticket(){
		return ++Manager::counter;
	}
	static int counter;
};
//��ôһ����Task��Schedule::dispatch()��ticket()��ģ��ʵ��������Manager����Ԫ

#endif