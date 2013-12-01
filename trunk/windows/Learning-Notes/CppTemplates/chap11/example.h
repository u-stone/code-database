#ifndef EXAMPLE_CHAP11_H
#define EXAMPLE_CHAP11_H

#include <iostream>
using namespace std;

//ģ��ʵ������
//���½�����ǹ���ģ��Ĳ������﷽��Ĺ���
//�����м�����Ҫ�Ĺ����ǣ�
//1. ʵ��-�βζԵķ����Ƕ�����
//2. ��ʹ����ƥ��ɹ�Ҳ��һ����������ɹ�
//3. �����ʱ����ĳЩ����»ᷢ��decayת�ͣ�����ͺ���ָ�����ʽתΪָ�����ͣ��������ʱ������const��volatile�޶���
//4. ���Ƕ������������ǲ��ᷢ��decay��

template <typename T>
T const& max_(T const& a, T const & b){
	return a<b?b:a;
}

void testT1(){
	//int g = max_(1, 1.0);//����error C2782: ��const T &max(const T &,const T &)��: ģ�� ������T������ȷ
	double g2 = max_<double>(1, 1.0);//��ȷ�����Ǿ��棺warning C4244: ����ʼ����: �ӡ�const double��ת������int�������ܶ�ʧ����
}
//ԭ����������ݶ������������Ľ����������յ�һ��������TӦ����int�����Ǹ��ݵڶ����������������Ľ����double����ʱ������������ì�ܣ�һ������T�����������ܵ�ֵ�����Ǿͱ�����

template<typename T>
typename T::Element at(T const& a, int i){//�����typename�������
	return a[i];
}

void f(int* p){
	//int x = at(p, 7);//������ΪTΪint*����ô������(int*)::Element������죬���Ǿ�ʧ���ˡ�����error C2893: δ��ʹ����ģ�塰T::Element at(const T &,int)��ר�û�
	//int x2 = at<int*>(p, 7);//���󣬱���error C2770: ��T::Element at(const T &,int)������ʽ ģ�� ������Ч
}

template <typename T>
void f(T){}

template <typename T>
void g(T&){}

double x[20];
int const seven = 7;

void testT2(){
	f(x);     //��ȷ��T��double*�����﷢����decay
	g(x);     //��ȷ���������ͣ�T��double[20]���������Ͳ��ᷢ��decay��
	f(seven); //��ȷ��T��int
	g(seven); //��ȷ���������ͣ�T��int const
	f(7);     //��ȷ��T��int
	//g(7);     //���󣬲��ܰ�7���ݸ�int&������error C2664: ��g��: ���ܽ����� 1 �ӡ�int��ת��Ϊ��int &��
}

//�����������Ͳ��ᷢ��decay���Կ�һ������
template <typename T>
T const& max1(T const& a, T const& b){
	return a;//����û�����Ƚ�
}
template <typename T>
T const max2(T const a, T const b){
	return a;//����û�����Ƚ�
}
void testT3(){
	//max1("apple", "pear");//���󣬱���
	/*
	error C2782: ��const T &max_(const T &,const T &)��: ģ�� ������T������ȷ
	g:\visual studio 2010\projects\cpptemplates\cpptemplates\chap11\example.h(53) : �μ���max_��������
	�����ǡ�const char [5]��
	��    ��const char [6]��

	�ɴ˿ɼ�apple��Ӧconst char[6]��pear��Ӧconst char[5]
	*/
	max2("apple", "pear");//������decay�����߶������char const*
}

//���ڸ������͵Ĳ�����ƥ������Ǵ���㿪ʼ���ϵݹ����ƥ��ģ����������һ�¡����㡱�͡��ݹ顱�ĺ���
template <typename T>
void f1(T* ){}

template <typename E, int N>
void f2(E(&)[N]){}

template <typename T1, typename T2, typename T3>
void f3(T1 (T2::*)(T3*)){}//����е�С���ӣ������������T2�����е�һ����Ա����ָ�룬����ֵ��T1������ֻ��һ������T3*

class S{
public:
	void f(double*){}
};

void testT4(){
	int *** ppp = NULL;
	bool b[42];
	f1(ppp);   //T��int**
	f2(b);     //T��bool��N��42
	f3(&S::f); //T1=void, T2=S,T3=double
}
//��������ĵݹ����ʣ����ݹ�������о�ͷ�ģ�����Щ������ǲ�����Ϊ���������ĵģ�
//1. ĳЩ�������͵����ƣ�����Q<T>::X���ܴ���T������T->Q<T>::X��Q<T>::X->Q<Q<T>::X>::X������չ�������޾�ͷ
//2. ���˷����͵Ĳ���֮�⣬ģ������к��������ɷֵķ����ͱ��ʽ�����磺S<I+2>�Ͳ��ܴ���I������I->S<I+2>��S<I+2>->S<S<I+2>+2>��Ҳ���޾�ͷ��
//3. ͬ2�����б��磺int(&)[sizeof(T)]���ܴ���T������ͬ��
template <int N>
class X{
public:
	typename int I;
	void f(int){
	}
};
template <int N>
void fppm(void (X<N>::*p)(typename X<N>::I)){
	
}
void testT5(){
	//fppm(&X<33>::f);//��������˵����˼N�Ǳ�ת��Ϊ33������VS2010����������˸��ӵ�ģ�壬����error C2893: δ��ʹ����ģ�塰void fppm(void (__thiscall X<N>::* )(X<N>::I))��ר�û�
	//˵������������˵��˼·�������з����������������X<N>::I�ǲ�������������ģ�����X<N>::*p�ĳ�Ա����X<N>�ǿ�������ģ�
	//      ���Ǿ͵õ��˲���N�������N���벻�������X<N>::I�У��͵õ�һ����ʵ��&X::fƥ������ͣ�������ʵ��-�β�����ͳɹ���
	//      ����������Ҫ˵������������Ĳ��������ǿ����ƹ��ģ���Ϊ����Ĺ��̲���Ψһ�ģ����Ƕ�·��������������������ֻҪƥ��ɹ��Ϳ��Եģ��������������������ġ�
}

//���ǣ���ʱ�����������ȫ�������������ĵ��������Ҳ���������������ӣ�
template<typename T1, typename T2>
class XX{
};
template <typename T>
class Y{};

template <typename T>
void f3(XX<Y<T>, Y<T>>){
	
}
void testT6(){
	f3(XX<Y<int>, Y<int>>());  //��ȷ
	//f3(XX<Y<int>, Y<char>>()); //���󣬱���
	/*
	error C2782: ��void f3(XX<Y<T>,Y<T>>)��: ģ�� ������T������ȷ
	g:\visual studio 2010\projects\cpptemplates\cpptemplates\chap11\example.h(124) : �μ���f3��������
	�����ǡ�char��
	��       ��int��

	���Ż�����
	error C2784: ��void f3(T1 (__thiscall T2::* )(T3 *))��: δ�ܴӡ�XX<T1,T2>��Ϊ��T1 (__thiscall T2::* )(T3 *)���Ƶ� ģ�� ����
	with
	[
	    T1=Y<int>,
	    T2=Y<char>
	]

	������������:���f3()���������������������ͬ��ʵ�����ͣ�����X��ͬ���͵���ʱ������
	*/
}


//���濴������������˵����ֻ����Ϊ���Ƿ��������ʱ�������ں�������
template <typename T>
void f4(T, T){}
void testT7(){
	void (*pf)(char, char) = &f4;//�����ģ��ʵ����void(char,char)���β���void(T,T)������char�滻T��
}

class S2{
public:
	//template<typename T, int N>
	//operator T[N]&();//���������л�Ҫ˵�ģ���ϧ��������֧������д��������ֻ����ͼ��˵���ɣ�
	                   //��׼��ʵ��֮��Ĺ�ϵ����ʱ�������������ʵ�Ĺ�ϵ��˵�õ���һ�����õ���
	void f(int){}
};
void f5(int(&)[20]);
void testT8(){
	S2 s;
	//f5(s);//����������˵����S2����ת��Ϊint(&)[20]����ô��f5()�Ĳ��������оͿ�����int���滻T,��20�滻N��
}

//���Ҳ�����ȷ��ʵ��-�β�ƥ���ʱ�򣬱������ῼ�ǽ������漸�ֿ��ɵ�ƥ��
template <typename T>
void f5(const T& a, volatile T& b){
	cout << "f5() const " << typeid(a).name() << "   " << typeid(b).name() << endl;
}
//template <typename T>
//void f5(/*const*/ T& a, /*volatile*/ T& b){
//	cout << "f5() non-const " << typeid(a).name() << "   " << typeid(b).name() << endl;
//}
void testT9(){
	S2 s1, s2;
	f5(s1, s2);//��ģ���β����������͵�ʱ��ʵ�ο��Բ��ÿ����βε�const��volatile�޶���
	//�����f5() const class S2   class S2
	//����⿪����ע�͵��ķ�const�������͵�f5()����ʱ���ͻᷢ����ȷƥ��
}

template <typename T>
void f6(T a, int i){
	cout << "int f6() " << i << "\n" 
		 << typeid(T).name() << endl;
	cout << typeid(a).name() << endl;
	cout << endl;
}

void testT10(){
	const S2 *ps1 = NULL;
	f6(ps1,1); 

	S2 tmps;
	S2 *const ps2 = &tmps;
	f6(ps2,2);//�����const

	volatile S2 *ps3 = NULL;
	f6(ps3,3);

	S2 *volatile ps4 = NULL;
	f6(ps4,4);//�����volatile

	void (__thiscall S2::*p)(int) = &S2::f;
	f6(p,5);
	/*
	�����
	int f6() 1
	class S2 const *
	class S2 const *

	int f6() 2
	class S2 *
	class S2 *

	int f6() 3
	class S2 volatile *
	class S2 volatile *

	int f6() 4
	class S2 *
	class S2 *

	int f6() 5
	void (__thiscall S2::*)(int)
	void (__thiscall S2::*)(int)
	*/
	//˵������ʵ��������ָ����߳�Աָ���ʱ�򣬲���ƥ������У�����������ѡ�����ʵ���е�const��volatile�޶������в�������
}

template <typename T>
class B{
};

template <typename T>
class D : public B<T>{
};

template<typename T>
class Chaos{
	template <typename T>
	operator B<T>(){}
};


template <typename T>
void f11(B<T>* ){
	cout << "in f11(B<T>*) : " << typeid(B<T>).name() << endl;
}
template <typename T>
void f12(B<T> b){
	cout << "in f12(B<T>*) : " << typeid(B<T>).name() << endl;
}



void testT11(){
	D<long> dl;
	f11(&dl);
	f12(dl);
	//Chaos<double> chd;
	//f12(&chd);//���󣺱���
	/*
	error C2784: ��void f12(B<T>)��: δ�ܴӡ�Chaos<T> *��Ϊ��B<T>���Ƶ� ģ�� ����
	with
	[
	    T=double
	]
	g:\visual studio 2010\projects\cpptemplates\cpptemplates\chap11\example.h(258) : �μ���f12��������
	*/

	//����������ת�������������ʱ���ǣ�
	//1. ������̲��漰��ת�������ģ��ʱ���βο�����ʵ�εĻ������ͣ�����
	//2. ��ʵ����ָ�����͵�ʱ���βο�����ʵ��ָ�����͵Ļ���

	//�����
	//in f11(B<T>*) : class B<long>
	//in f12(B<T>*) : class B<long>
}

//����̸����ȫ���Ǻ��������ǳ�Ա�����Ĳ������������ܹ�����ģ��Ĳ������ֻ��һ����
// �������ﲻ��������ģ�壬������ģ��Ĺ��캯��Ҳ���ܸ���ʵ����������ģ�����
template <typename T>
class S3{
public:
	S3(T b) : a(b){}
private:
	T a;
};

void testT14(){
	//S3 s(12);//���󣬱���error C2955: ��S3��: ʹ���� ģ�� ��Ҫ ģ�� �����б�
	         //����Ϊ��ģ����в�������

	S3<int> s3(12);//��ȷ����ʾʵ����
}

//����ȱʡ��������ʵ��
template <typename T>
void init(T* loc, T const& val = T()){
	*loc = val;
}
class S4{
public:
	S4(int, int){}
};
void testT15(){
	S4 s(0,2);
	init(&s, S4(1,2));//��ȷ����ȻS4û��Ĭ�Ϲ��캯��������T()����Ч�ˣ���������������ʱ��ֱ��ʹ������ʾ�����Ĳ�����
}
//ͬ������ģ�岻��ʹ�ò����������ȱʡ�����ĵ��������µ�Ӱ��
template <typename T>
void f15(T x = 28){
}
void testT16(){
	f15<int>(); //T=int
	//f15();      //���󣬲��ܸ���28���Ƶ�T������error C2783: ��void f15(T)��: δ��Ϊ��T���Ƶ� ģ�� ����
}

//���һ����Ԫ�����Ķ��壨����ԭ��˵����Barton-Nackman������������Ϊһ����ʱ�ļ���������˵��������Ҫ�о�������������û�������ߵ���ͼ��
class S5{};

template <typename T>
class Wrapper{
private:
	T object;
public:
	Wrapper(T obj) : object(obj){}//����һ����ʽת��T��Wrapper��
	friend void f16(Wrapper<T> const & a){
		cout << "In f16() " << typeid(a).name() << endl;
	}
};
void testT17(){
	S5 s;
	//f16(s);//��������������ǲ���ͨ������ģ�ͨ��ADL��������S5�Ļ�����ⲿ���������ҵ�f6()�Ķ��壬����error C3861: ��f16��: �Ҳ�����ʶ��
	Wrapper<S5> w(s);
	f16(w);//��ȷ������ͨ��w��Wrapper<S5>�����������ҵ�f16()�Ķ���
	f16(s);//�������������Ϳ���ͨ�����룬����������������һ����ģ��ʵ����POIʱ���ɵ����ݡ�
	//�����
	/*
	In f16() class Wrapper<class S5>
	In f16() class Wrapper<class S5>
	*/
}

#endif