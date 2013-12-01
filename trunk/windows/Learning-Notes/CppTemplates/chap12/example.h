#ifndef EXAMPLE_H_CHAP12_
#define EXAMPLE_H_CHAP12_

#include <iostream>
using namespace std;

//��һ������ģ���ػ���ģ�庯��������

//���ʹ����ṩ��һ���׵Ĳ������̣�������ʱ�����������ĳЩ������һЩ����Ĳ��������Ч�ʣ����ߴ��������Ŀ�ģ���ʱ�����Ҫ����ģ���ػ�
//��������ͨ��һ�����ӿ���ͨ���������ش������Ż�
#include "ArrayClass.h"

template <typename T>
inline void quick_exchange(T* a, T* b){
	cout << "in quick_exchange(T* a, T* b)" << endl;
	if (a == NULL || b == NULL)
		return ;
	T tmp(*a);
	*a= *b;
	*b = tmp;
}

template <typename T>
inline void quick_exchange(Array<T>* a, Array<T>* b){
	cout << "in quick_exchange(Array<T>* a, Array<T>* b)" << endl;
	if (a == NULL || b == NULL)
		return ;
	a->exchange_with(b);
}

void testT1(){
	int x = 1, y = 2;
	Array<int> *p1 = NULL, *p2 = NULL;
	quick_exchange(&x, &y);
	quick_exchange(p1, p2);
	//�����
	//in quick_exchange(T* a, T* b)
	//in quick_exchange(Array<T>* a, Array<T>* b)
}
//��testT1()�У�����p1��p2Ϊʵ�εĵ���������汾��ģ�壬���������������е��ٶȡ�

//�ھ�������������ڴ���������ʱ�����õĹ���ǰ�������ȿ�һ������������quick_exchange()�ڵ��õ�ʱ������Ĳ�ͬ�ĸ�����
struct S1{
	int x;
}s1, s2;

void distinguish(Array<int> a1, Array<int> a2){
	int * p = &a1[0];
	int * q = &s1.x;
	a1[0] = s1.x = 1;
	a2[0] = s2.x = 2;
	cout << "before: *p = " << *p << endl;
	cout << "before: *q = " << *q << endl;
	quick_exchange(&a1, &a2);
	cout << "after: *p = " << *p << endl;
	quick_exchange(&s1, &s2);
	cout << "after: *q = " << *q << endl;
}

void testT2(){
	int parg1[] = {1, 2, 3};
	int parg2[] = {4, 5, 6};
	Array<int> a1(parg1), a2(parg2);
	distinguish(a1, a2);
	//�����
	//before: *p = 1
	//before: *q = 1
	//after: *p = 1
	//after: *q = 2
	//mark : ����distinguish()��p, q������ָ���ڴ��a1��s1���׵�ַ�ġ�����������ʱ�鿴�����ַ�е����ݣ�
	//       ���ǲ鿴�Ľ���ǲ�ͬ��quick_exchange()����֮�󣬶��ڴ�Ĳ����ǲ�ͬ�ġ�
	//       ��ʱ������ں�������֮�����������ͬ�Ķ�Ӧλ�õ��ڴ棬���ǽ����ͬ�������������ֱ�ۣ��Ͼ����õĺ�����������ͬ�ġ�
	//       ������ڴ�����Ĳ�ͬ�������ǰ���ᵽ�Ĳ�ͬ�ĸ����ã����߽б�ԵЧӦ
}

//�������ص����⣬��һ��ģ�庯��������

//�ȿ���ʾʵ��������
template <typename T>
int f(T){
	return 1;
}

template <typename T>
int f(T* ){
	return 2;
}

void testT3(){
	cout << "f<int*>((int*)0) = " << f<int*>((int*)0) << endl;
	cout<< "f<int>((int*)0) = " << f<int>((int*)0) << endl;
	//�����
	//f<int*>((int*)0) = 1
	//f<int>((int*)0) = 2
}
//��������Ҫע���������f()���������ܻ������ͬ��ʵ�������룬������int*�����һ��f()�е�T��ʹ��int����ڶ���f()�е�T���ͻ������ͬ�ĺ�����
//������Ҳû�й�ϵ������˵������������ͬ���Ƶ�ģ��ͬʱ���ڣ�Ҳ������ͬ��ʵ������ͬʱ���ڣ����ǡ���˵��ǰ�桱������Ĺ����ǲ��ܱ�֤���������ض����Ե�
/*
  C++�й涨��ֻҪ�в�ͬ�ĺ���ǩ���������Ϳ���ͬʱ���ڡ�������ǩ������Ϊ��
  1. �����޵ĺ������ƣ�Ҳ�����ǲ����Ժ���ģ�����������
  2. �������������������������ֿռ�������������������Ǿ����ڲ����ӵģ����������������ڵķ��뵥Ԫ
  3. ���Ա�����е�const��volatile��const volatile�޶���
  4. �����Ĳ������ͣ�����ǲ����Ժ�����ģ�壬��ô����˵����ģ������滻֮ǰ��
  5. ����������ʱ�����Ժ���ģ�壬��ô��������ֵ��ģ�庯�����βκ�ʵ��
*/

//����������˵����
//����1����ǵ�Ȼ�ģ���ͬ�ĺ������Ƶ�Ȼǩ���ǲ�ͬ�ġ�
//����2Ҳ��һ�����������ͨ������ǩ��һ��
/////////////////////////����3/////////////////////////////////////////////////
template <typename T>
class C2{
public:
	void f(T a){}
	void f(T a)const{}
	void f(T a)volatile{}
	void f(T a)const volatile{}
};
//����˵�������������η��Ǳ�����������е�
template <typename T>
void f1(T){}

// template <typename T>
// void f1(T) const{}
//����error C2270: ��f1��: �ǳ�Ա�����ϲ��������η�

// template <typename T>
// void f1(T)volatile{}
//����error C2270: ��f1��: �ǳ�Ա�����ϲ��������η�

//����volatile���η����Բο���ƪ���£����volatile��C++�е�����(http://www.cnblogs.com/s5279551/archive/2010/09/19/1831258.html)

////////////////////////����4//////////////////////////////////////////////////
template <typename T1, typename T2>
void f1(T1, T2){}

template <typename T1,typename T2>
void f1(T2, T1){}
////////////////////////����5//////////////////////////////////////////////////
template <typename T>
long f2(T){}

template <typename T>
char f2(T){}
//////////////////////////////////////////////////////////////////////////

//�ٿ�һ������������������Ե�����
void testT4(){
	//f1<char, char>('a', 'b');//���󣬱���error C2668: ��f1��: �����غ����ĵ��ò���ȷ
	/*
	g:\visual studio 2010\projects\cpptemplates\cpptemplates\chap12\example.h(138): �����ǡ�void f1<char,char>(T2,T1)��
	with
	[
	    T2=char,
	    T1=char
	]
	g:\visual studio 2010\projects\cpptemplates\cpptemplates\chap12\example.h(135): ��       ��void f1<char,char>(T1,T2)��
	with
	[
	    T1=char,
	    T2=char
	]
	*/
}
//��������Ƿ��ڲ�ͬ�ķ��뵥Ԫ����ô���Խ��������⣬���������˹���2��
//template <typename T1, typename T2>
//void f3(T1, T2){}
//
//extern void g();
//
//void testT5(){
//	f3<char, char>('a', 'b');
//	g();
//}//VS2010�ϱ��벻ͨ��

//���������ǿ�һ�����غ����ڵ���ѡ��ʱ������������ֹ����ʹ�ó�����һ��ģ�庯������ʱ�ĺ���ѡ���ϡ�
//������һ��ģ�庯���õ����أ����ﲻ����ȱʡ�βκ�ʡ�Ժ��β��������ǳ���������ģ�庯��ft1(), ft2()��
//��������������б�ֱ��Ӧ����ģ�庯����A1��A2����ô����˵A1ƥ��ft1��A2ƥ��ft2����ʱ���ٿ��ǽ���ƥ��Ĺ�ϵ�������
//    ft1ƥ��A2������ft2��ƥ��A1����ô���Ǿ�˵ft2Ҫ��ft1�������⡣���ڽ��в��������ʱ���������ѡ���������ĺ�������Ϊ�������ĳ����Ϊ����
template <typename T>
void fnc(T*, T const* = 0, ...){
	cout << "first function" << endl;
}
template <typename T>
void fnc(T const*, T*, T* = 0){
	cout << "second function" << endl;	
}
void testT6(){
	int *p = 0;
	//fnc(p, p);//����error C2666: ��fnc��: 2 �����������Ƶ�ת��
}
//������ʵ���б���Գ���Ϊ(A*, A*)���β��б���Գ���Ϊ(A1*, A1 const*)��(A2 const*, A2)��ȱʡ�Ĳ�����ʡ�ԺŲ���const
//��������ָ�����͵Ĳ��������ڲ��������ʱ�����const��volatile֮����޶�������ƥ�䡣����������ʵ�ζ����Խ��в���ȷ��ƥ�䣬����û�и��ر��ģ�庯������Ϊ���������Ƕ����ģ�����˳��

//�ٲ���һ�㣬�������صĺ���ģ��ľֲ�������򣬵�����ʵ�������ʱ�����ĵ��û�������ѡ��
void testT7(){
	cout << f(0) << endl;          //����f(T)
	cout << f((int*)0) << endl;    //����f(T*)
	//f(0)�в���������int������f(T)������f(T*)������һ��ָ�룬����֮��ֻ��ѡ��f(T)
	//f((int*)0)�����ʱ����������������ƥ�䣬f<int>(int*), f<int*>(int*)�����ԣ���������ѡ����������ģ�庯��
}

//��ʽ�ػ�
//��Ҫ�ڶ���֮ǰ���� template<> ǰ׺

//�ȿ���ģ��ȫ���ػ�
template <typename T>
class S{
public:
	void info(){
		cout << "generic (S<T>::info())" << endl;
	}
};
//�����Ǳ�׼��ģ�嶨�壬�����Ǹ���void���͵���ʽ�ػ�
template <>
class S<void>{
public:
	void msg(){
		cout << "fully specialized (S<void>::msg())" << endl;
	}
};
//���Կ���������ػ��У���ģ��ĳ�Ա�����ǲ�һ���ģ�ʵ����ȫ���ػ�ֻ����ģ��������йء�
//���������м�������
//1. �����÷�����ģ��ʵ�����滻ģ�����Ͳ���
//2. ���ģ���������ȱʡ��ģ��ʵ�Σ���ô�����滻��ģ��ʵ�ξ��ǿ�ѡ��
template <typename T>
class Types{
public:
	typedef int I;
};
template <typename T, typename U = typename Types<T>::I>
class S2;       //����(a)

template <>
class S2<void>{//����(b)
public:
	void f();
};

template <> 
class S2<char, char>;//����(c)

//template <> class S2<char, 0>;//���󣬱���error C2974: 'S2' : ģ�� ���� 'U'����Ч������ӦΪ����
                                //��Ϊ����1��������0���滻U

void testT8(){
	S2<int>* pi;  //��ȷ��ʹ�ö���(a)
	//S2<int>  el;  //����ʹ�ö���(a)������Ҫ�ж��壬ȴ�Ҳ�������
	              //����error C2079: ��el��ʹ��δ����� class��S2<T>��
	S2<void>* pv;  //��ȷ��ʹ�ö���(b)
	S2<void, int> sv; //��ȷ��ʹ�ö���(b)��ȱʡ����ʹ�õ��Ƕ���(a)��
	//S2<void, char> e2; //����ʹ�ö���(a)
	//S2<char, char> e3; //����ʹ�ö���(c)�������Ҳ������壨�ں����أ�
}

template <>
class S2<char, char>{};//ǰ������(c)�Ķ���

//������ģ���ػ��г�Ա�����Ķ��壬���Բ�����ͨ���������ֻ���ػ��������﷨����ƥ��ģ���������Ҳ���ǲ���ָ��template<>
template <typename T>
class S3;

template <>
class S3<char**>{
public:
	void print()const;
};
void S3<char**>::print()const{//���ã���������Ķ���֮ǰû�м�ģ���ػ���ǰ׺template<>
	cout << "pointer to pointer to char" << endl;
}

//������һ�����Ӹ��ӵ�������˵������
template <typename T>
class Outside{
public:
	template <typename U>
	class Inside{};
};

template<>
class Outside<void>{
	template<typename  U>
	class Inside{
	private:
		static int count;
	};
};
template <typename U>//���ǰ׺�Ǹ�Inside�ӵ�
int Outside<void>::Inside<U>::count = 1;//����Ķ���Ҳû��ʹ��template<>ǰ׺

//���￼��һ�����⣺ȫ��ģ���ػ����ܻ�����һ���뷺��ģ��ʵ�������ɵİ汾������Ľ���취���ǲ��������߳�����ͬһ���ļ���
template <typename T>
class Invalid{};

Invalid<double> x1;//����һ��ʵ������:

//template <>
//class Invalid<double>;//���󣬱���error C2908: ��ʽר�û�����ʵ������Invalid<T>��

//tips:���ǵ�����������������������뵥Ԫ�е�ʱ��Ͳ����ױ����֣���ʱ��ȽϿ��еİ취���ǽ��ػ�������ڷ��Ͷ���֮�󣬷���һ�������Ͳ�����ֲ��ױ����ֵ����

//����û�а���ģ�嶨�����һ���ɾ����ļ��У����ں��渽���ػ��汾����������һ������������ļ��ж������ػ��汾��
//����ʹ�õ�ʱ��ͻ�����ͷ�ļ�����˳������⣬���п�����ʹ�÷��Ͷ���Ŀ�����

#include "template_class2.h"//����ļ��������template_class.h��ǰ�棬�������������ļ��õ��Ľ����Ƿ��͵Ķ���
#include "template_class.h"


void testT8_5(){
	testBuffer();
	//clear(buffer);
	//����� Danger<void>::max = 100
}

//�����������ȷ�����ӣ���ʱ���ػ��Ķ��壬ͷ�ļ��İ����Ϳ���������(����ʹ��CHOICE��Ϊ���أ��������������)
#define CHOICE

#ifdef CHOICE

	template <typename T>
	class Correct;

	template <>
	class Correct<void>{//�ػ�
	public:
		enum {value = 28};
	};
	#include "template_correct_class.h"

#else

	#include "template_correct_class.h"

	//ͷ�ļ�������ǰ���Ͳ���Ҫ�������ģ��������
	//template <typename T>
	//class Correct;

	template <>
	class Correct<void>{//�ػ�
	public:
		enum {value = 28};
	};

#endif


void testT8_5_0(){
	cout << "Correct<void>::value : " << endl;
	cout << Correct<void>::value << endl;
	cout << Correct<int>::value << endl;
	//�����
	/*
	Correct<void>::value :
	28
	14
	*/
}

//�������ʹ�õķ��Ͷ��岻�����Լ���д��ʱ��
//���Խ�������ͷ�ļ���һ���Լ���ͷ�ļ�������������д���Լ����ػ��汾��ͨ�����ְ�װʹ������Ļ���

//���濴ȫ�ֵĺ���ģ���ػ�
//��Ƚ���ģ����ػ�������ģ���ػ��¼�������������غ�ʵ������
//����˵������������ģ����ػ�
template <typename T>
int f4(T){
	return 1;
}
template <typename T>
int f4(T* ){
	return 2;
}
//f4(T)���ػ�
template <>
int f4(int){
	return 3;
}
//f4(T*)���ػ�
template <>
int f4(int*){
	return 4;
}
//ȫ�ֺ���ģ���ػ����������ȱʡʵ��ֵ�������ػ��汾�ĺ���ģ���ʹ�û���ģ�������ȱʡʵ��ֵ
template <typename T>
int f5(T, T x = 42){
	return x;
}
//template <>
//int f5(int, int = 35){//���󣬱���error C2765: ��f5��: ����ģ�����ʽר�û��������κ�Ĭ�ϲ���
//	return 0
//}

template <typename T>
int g(T, T x = 42){
	return x;
}

template <>
int g(int, int y){
	return y/2;
}

void testT9(){
	cout << g(0) << endl;
	//���:  21
	//������g(int, int y)��һ�ػ��汾����ʹ���˻���ģ���ȱʡʵ��
}

//ȫ���ػ���������ͨ���������ƣ����԰�ȫ���ػ�����������һ����ͨ���ٴ�������ȫ���ػ�����������������һ��ģ�塣
//��������������ģ���ػ���������ͬ�����⣬��������ȫ�ֺ���ģ���ػ�ֻ�ܶ���һ�Σ�����Ҫ������ģ��ֱ�������ػ��汾�ĺ�����
//����������������
//һ�������ǣ����ػ��汾�ġ����������ڽ�������ģ����棬����һ���ļ�A�У������ػ��汾�Ķ��������һ���ļ�B��
//            ���������Ҳֱ�ӷ��ڻ���ģ�嶨����ļ��У���ô��Ȼ�Ƿ��ڻ���ģ�嶨��֮�󣬲�����ʱ���ػ��汾�ĺ��������õ�
#include "template_fn.h"//���ͷ�ļ����� fn��ģ���fn���ػ�����

void print(){
	fn(1,2);//���template_fn.h��û��fn�ػ�����������ô���оͻ�����һ������
	        //error C2908: ��ʽר�û�����ʵ������int fn<int>(T,T)��
}

//������fn���ػ��汾����
template<>
int fn(int, int y){
	return y/2;
}

void testT10(){
	cout << fn(0) << endl;
	//���14
}

//��һ�ְ취�ǽ�������ڻ���ģ�壬���Ǳ��뽫������Ϊinline����
#include "template_fn2.h"  //���ļ��ж�����fn2�Ļ���ģ��������汾�ĺ����ػ�����

void testT11(){
	cout << fn2(0, 30) << endl;
	//����� 15
}

//���濴 ȫ�ֳ�Ա�ػ�
template <typename T>
class Outer{
public:
	template <typename U>
	class Inner{
	private :
		static int inner_value;
	};
	static int outer_value;
	void print()const{
		cout << "generic" << endl;
	}
};

template <typename T>
int Outer<T>::outer_value = 6;//���Ͱ汾�ĸ�ֵ

template <typename T>
template <typename U>
int Outer<T>::Inner<U>::inner_value = 7;//���Ͱ汾���ڲ���ľ�̬��Ա��ֵ����Ҫд����templateǰ׺

//������Outer��bool���ػ�
template <>
class Outer<bool>{
public:
	template<typename U>
	class Inner{
	private:
		static int Inner_value;
	};
	void print()const{//�ػ��汾�ĳ�Ա������д
		cout << "specific" << endl;
	}
};
//��������Outer���Ա��ȫ���ػ�
template <>
int Outer<void>::outer_value = 12;
//��������Կ�����������ͨ��ĳ�Ա�����;�̬�������ԣ�����������C++���ǲ�����ģ����Ƕ�����ģ���ػ���Ա�����ǺϷ���
template <>
void Outer<void>::print()const{
	cout << "Outer<void>" << endl;
}
//�����Ա�ػ������������ڶ��壬������Щ�����Ա���Ϊ����(Ϊ�����ظ���ע�͵���)
//template <>
//int Outer<void>::outer_value;
//template <>
//void Outer<void>::print()const;

//�����Ҫ��Outer<T>::Inner����Ա�ػ�������������
template <>
template<typename X>
class Outer<wchar_t>::Inner{
public:
	static long inner_value;//Outer<wchar_t>�汾��Ա�ػ���Inner�е�inner_value�����long����
};

template <>
template <typename X>
long Outer<wchar_t>::Inner<X>::inner_value;
//�ⲿ���ػ����ڲ�������ģ�壬������ⲿ���ȫ�ֳ�Ա�ػ�

//�����������ڲ�����ػ�����Ȼ�ڲ�����ػ�ֻ�����ⲿ���ĳһ���ػ�ʱ���ػ�
template <>
template <>
class Outer<char>::Inner<wchar_t>{
public:
	enum {inn = 1};
};

//�����д���Ǵ���ģ�
//template <typename X>
//template <>
//class Outer<X>::Inner<void>;//����error C3212: ��Outer<T>::Inner<void>��: ģ���Ա����ʽר�û���������ʽר�û��ĳ�Ա

//�����Ѿ��ػ�����Outer<bool>���͵��ڲ����ػ���������д��
template <>         //ֻ��Ҫһ��template<>ǰ׺�����ٴ����������ǿ�����Ϊ�ػ������������Ϊ��ͨ��
class Outer<bool>::Inner<wchar_t>{
public:
	enum {inn = 2};
};

//�����д������Ҳ�ǺϷ��ģ�
class DefaultInitOnly{
public:
	DefaultInitOnly(){}
private:
	DefaultInitOnly(DefaultInitOnly const&);//�����ڿ�������
};

template <typename T>
class Statics{
private:
	static T sm;
};
//ΪStatics��DefaultInitOnly�����ṩһ��ȫ���ػ�����
DefaultInitOnly obj;
template <>
DefaultInitOnly Statics<DefaultInitOnly>::sm;

//���濴�ֲ�����ģ���ػ�
//�������������е�ʱ�����ǲ��������������ģ����Ƴ�һ����Ծ���ʵ���б��ȫ���ػ���
//����ϣ������ģ���ػ���һ�����ģ��ʵ�ε�����塣���߼򵥵�˵����������ģ�����ĳһ�������ݹ���һ��ʵ��
template <typename T>
class List{
public:
	//.....
	void append(T const&);
	inline size_t lenght()const;
	//.....
};

//���һ��������ʹ���˴����Ĳ�ͬ���͵�List����ô�⽫��ʵ�������ܶ��µĴ��룬�Ӷ��Ӵ����������������ϸ���Ļ�������append
//����ͬ����ָ�����͵�append�������������ȫ��ͬ�ģ�����˵����ϣ����һ��ָ������List����ͬһ��ʵ�֡�
//��ô���ǿ����ػ�һ������ָ�����͵�List��

//��������Ǹ��ݺ����������ģ����������
template<>
class List<void*>{
	void append(void* p);
	inline size_t length()const;
};


template <typename T>
class List<T*>{//�µ��ص�������<T*>
private:
	List<void*> impl;
	//....
public:
	void append(T* p){
		impl.append(p);
	}
	size_t length()const{
		return impl.length();
	}
};
//��Ϊ���ģ���ʵ��ֻ�Ǳ�����ʹ�ã����Ա������ֲ��ػ�
//�ֲ��ػ����﷨�ǣ�ǰ����һ��template<...>�������б�����ģ������ƺ�����ʽָ��ģ���ʵ���б����������<T*>
//���Ǹ��ݵ�11�������Ĳ��������ʱ�򣬲�����Ǳ�ڵ�����������ڣ���List<void*>������List<void*>����������ʹ������⡣��������Ӧ���ȶ���һ��void*��ȫ���ػ�
//template<>
//class List<void*>{
//	void append(void* p);
//	inline size_t length()const;
//};//��δ�����Ҫ����������ľֲ��ػ�ǰ��

//����������������ݹ������

//������һЩ�ֲ��ػ���Լ����
//1. ���ͺͷ����͵�ƥ��
//2. �ֲ��ػ������б�����ȱʡʵ�Σ��ֲ��ػ��Ի�ʹ�û�����ģ���ȱʡʵ��
//3. �ֲ��ػ��ķ�����ʵ��ֻ���Ƿ�����ֵ���������������ͱ��ʽ
//4. �ֲ��ػ���ģ��ʵ���б��ܺȻ�����ģ������б���ȫ��ͬ����һ���ھɵı���������Լ�����������µı�������û�����Լ��
template <typename T, int I = 3>
class S4;

//template <typename T>
//class S4<int, T>;          //����Υ��Լ��1  ����error C2975: ��S4��:��I����ģ�������Ч��ӦΪ����ʱ�������ʽ

//template <typename T = int>
//class S4<T, 10>;            //����Υ��Լ��2   ����error C2756: ��S4<T,10>��: ����ר�û��в�������Ĭ�ϲ���

//template <int I>
//class S4<int, I * 2>;       //����Υ��Լ��3   ����error C2755: ��S4<int,I*2>��: ����ר�û��ķ����Ͳ��������Ǽ򵥱�ʶ��

template <typename U, int K>
class S4<U, K>;             //vc2010����ȷ��������gcc����ΪΥ��Լ��4������������������S<U, K>����δ�������κ�ģ�����

//�ֲ�ģ��Ҳ��ͻ���ģ�巢����������ʹ��һ��ģ���ʱ�򣬱�������Ի���ģ����в��ң�
//���ǽ�������ƥ�����ʵ�κ�����ػ���ʵ�Ρ�������ݺ����غ���ģ��ѡ��ԭ��һ����ѡ��һ����������ػ�������ж��һ�����ػ��ͻ����������

//���Ҫָ�����ǣ���ģ��ֲ��ػ��Ĳ����������Ժͻ���ģ�岻һ����
//��������ʾһ�ֳ�Աָ�����͵��ػ�
template <typename C>
class List<void *  C::*>{//���ָ���Աָ���ָ��
public:
	//ָ��void*�ĳ�Աָ����ػ���Ϊ�˺�����������͵�����Ϸ�������
	typedef void * C::* ElementType;
	//...
	void append(ElementType pm);
	inline size_t length()const;
};

template <typename T, typename C>  //ע�⣬����Ĳ�������������Ĳ�һ��
class List<T* C::*>{//�ֲ��ػ�
private:
	List<void* C::*> impl;
public:
	//����κ�ָ���Աָ���ָ������(T*)�ľֲ��ػ�
	//void*���͵ĳ�Աָ��������ǰ���Ѿ�����
	typedef T* C::*ElementType;

	void append(ElementType pm){
		impl.append((void * C::*)pm);
	}

	inline size_t length()const{
		return impl.length();
	}
};

void testT12(){
	List<void*> p;
}


#endif