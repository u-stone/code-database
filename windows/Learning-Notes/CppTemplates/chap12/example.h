#ifndef EXAMPLE_H_CHAP12_
#define EXAMPLE_H_CHAP12_

#include <iostream>
using namespace std;

//这一章主讲模板特化和模板函数的重载

//泛型代码提供了一整套的操作流程，但是有时候我们想针对某些类型做一些特殊的操作来提高效率，或者达成其他的目的，这时候就需要进行模板特化
//下面我们通过一个例子看看通过函数重载带来的优化
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
	//输出：
	//in quick_exchange(T* a, T* b)
	//in quick_exchange(Array<T>* a, Array<T>* b)
}
//在testT1()中，数组p1，p2为实参的调用了数组版本的模板，这样提议提升运行的速度。

//在具体分析编译器在处理函数重载时所采用的规则前，我们先看一下上面这两个quick_exchange()在调用的时候带来的不同的副作用
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
	//输出：
	//before: *p = 1
	//before: *q = 1
	//after: *p = 1
	//after: *q = 2
	//mark : 函数distinguish()中p, q是用来指向内存段a1和s1的首地址的。这样可以随时查看这个地址中的数据，
	//       但是查看的结果是不同的quick_exchange()调用之后，对内存的操作是不同的。
	//       这时候，如果在函数调用之后如果访问相同的对应位置的内存，但是结果不同，这样会有悖于直观，毕竟调用的函数名字是相同的。
	//       上面对内存操作的不同结果就是前面提到的不同的副作用，或者叫边缘效应
}

//接下来回到正题，看一下模板函数的重载

//先看显示实例化调用
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
	//输出：
	//f<int*>((int*)0) = 1
	//f<int>((int*)0) = 2
}
//在这里需要注意的是两个f()函数，可能会产生相同的实例化代码，比如用int*代替第一个f()中的T，使用int代码第二个f()中的T，就会产生相同的函数，
//但是这也没有关系。就是说编译器允许相同名称的模板同时存在，也允许相同的实例化体同时存在，但是“丑话说在前面”，下面的规则是不能保证不出现重载二义性的
/*
  C++中规定，只要有不同的函数签名，函数就可以同时存在。函数的签名定义为：
  1. 非受限的函数名称，也可以是产生自函数模板的这类名称
  2. 函数名称所属的类作用域、名字空间作用域；如果函数名称是具有内部链接的，还包括该名称所在的翻译单元
  3. 类成员函数中的const、volatile，const volatile限定符
  4. 函数的参数类型，如果是产生自函数的模板，那么这里说的是模板参数替换之前的
  5. 如果这个函数时产生自函数模板，那么包括返回值，模板函数的形参和实参
*/

//下面做举例说明：
//规则1这个是当然的，不同的函数名称当然签名是不同的。
//规则2也是一样，这个跟普通函数的签名一样
/////////////////////////规则3/////////////////////////////////////////////////
template <typename T>
class C2{
public:
	void f(T a){}
	void f(T a)const{}
	void f(T a)volatile{}
	void f(T a)const volatile{}
};
//下面说明，这两个修饰符是必须出现在类中的
template <typename T>
void f1(T){}

// template <typename T>
// void f1(T) const{}
//报错：error C2270: “f1”: 非成员函数上不允许修饰符

// template <typename T>
// void f1(T)volatile{}
//报错：error C2270: “f1”: 非成员函数上不允许修饰符

//关于volatile修饰符可以参考这篇文章：详解volatile在C++中的作用(http://www.cnblogs.com/s5279551/archive/2010/09/19/1831258.html)

////////////////////////规则4//////////////////////////////////////////////////
template <typename T1, typename T2>
void f1(T1, T2){}

template <typename T1,typename T2>
void f1(T2, T1){}
////////////////////////规则5//////////////////////////////////////////////////
template <typename T>
long f2(T){}

template <typename T>
char f2(T){}
//////////////////////////////////////////////////////////////////////////

//再看一个上述规则产生二义性的例子
void testT4(){
	//f1<char, char>('a', 'b');//错误，报错：error C2668: “f1”: 对重载函数的调用不明确
	/*
	g:\visual studio 2010\projects\cpptemplates\cpptemplates\chap12\example.h(138): 可能是“void f1<char,char>(T2,T1)”
	with
	[
	    T2=char,
	    T1=char
	]
	g:\visual studio 2010\projects\cpptemplates\cpptemplates\chap12\example.h(135): 或       “void f1<char,char>(T1,T2)”
	with
	[
	    T1=char,
	    T2=char
	]
	*/
}
//如果将他们放在不同的翻译单元，那么可以解决这个问题，这是利用了规则2，
//template <typename T1, typename T2>
//void f3(T1, T2){}
//
//extern void g();
//
//void testT5(){
//	f3<char, char>('a', 'b');
//	g();
//}//VS2010上编译不通过

//接下来我们看一下重载函数在调用选择时的排序规则，这种规则的使用出现在一个模板函数调用时的函数选择上。
//假设有一个模板函数得到重载，这里不考虑缺省形参和省略号形参数，于是出现了两个模板函数ft1(), ft2()。
//如果有两个参数列表分别对应两个模板函数：A1，A2。那么就是说A1匹配ft1，A2匹配ft2，这时候再考虑交叉匹配的关系，如果：
//    ft1匹配A2，但是ft2不匹配A1，那么我们就说ft2要比ft1更加特殊。而在进行参数演绎的时候编译器会选择更加特殊的函数。因为这代表了某种人为倾向。
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
	//fnc(p, p);//报错：error C2666: “fnc”: 2 个重载有相似的转换
}
//分析：实参列表可以抽象为(A*, A*)，形参列表可以抽象为(A1*, A1 const*)和(A2 const*, A2)，缺省的参数和省略号参数const
//但是由于指针类型的参数可以在参数演绎的时候加上const，volatile之类的限定符进行匹配。所以这两个实参都可以进行不精确的匹配，并且没有更特别的模板函数。因为参数演绎是独立的，不管顺序。

//再补充一点，关于重载的函数模板的局部排序规则，当发生实参演绎的时候函数的调用会这样来选择
void testT7(){
	cout << f(0) << endl;          //调用f(T)
	cout << f((int*)0) << endl;    //调用f(T*)
	//f(0)中参数类型是int，符合f(T)，但是f(T*)类型是一个指针，演绎之后只能选择f(T)
	//f((int*)0)演绎的时候两个函数都可以匹配，f<int>(int*), f<int*>(int*)都可以，但是这里选择更加特殊的模板函数
}

//显式特化
//需要在定义之前加上 template<> 前缀

//先看类模板全局特化
template <typename T>
class S{
public:
	void info(){
		cout << "generic (S<T>::info())" << endl;
	}
};
//上面是标准类模板定义，下面是该类void类型的显式特化
template <>
class S<void>{
public:
	void msg(){
		cout << "fully specialized (S<void>::msg())" << endl;
	}
};
//可以看出上面的特化中，类模板的成员函数是不一样的，实际上全局特化只和类模板的名称有关。
//不过这里有几个规则：
//1. 不能用非类型模板实参来替换模板类型参数
//2. 如果模板参数具有缺省的模板实参，那么用来替换的模板实参就是可选的
template <typename T>
class Types{
public:
	typedef int I;
};
template <typename T, typename U = typename Types<T>::I>
class S2;       //定义(a)

template <>
class S2<void>{//定义(b)
public:
	void f();
};

template <> 
class S2<char, char>;//定义(c)

//template <> class S2<char, 0>;//错误，报错：error C2974: 'S2' : 模板 对于 'U'是无效参数，应为类型
                                //因为规则1，不能用0来替换U

void testT8(){
	S2<int>* pi;  //正确，使用定义(a)
	//S2<int>  el;  //错误，使用定义(a)但是需要有定义，却找不到定义
	              //报错：error C2079: “el”使用未定义的 class“S2<T>”
	S2<void>* pv;  //正确，使用定义(b)
	S2<void, int> sv; //正确，使用定义(b)，缺省参数使用的是定义(a)的
	//S2<void, char> e2; //错误，使用定义(a)
	//S2<char, char> e3; //错误，使用定义(c)，但是找不到定义（在后面呢）
}

template <>
class S2<char, char>{};//前置声明(c)的定义

//对于类模板特化中成员函数的定义，可以参照普通类的声明，只是特化声明的语法必须匹配模板的声明，也就是不能指定template<>
template <typename T>
class S3;

template <>
class S3<char**>{
public:
	void print()const;
};
void S3<char**>::print()const{//看好，这个函数的定义之前没有加模板特化的前缀template<>
	cout << "pointer to pointer to char" << endl;
}

//下面有一个更加复杂的例子来说明问题
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
template <typename U>//这个前缀是给Inside加的
int Outside<void>::Inside<U>::count = 1;//这里的定义也没有使用template<>前缀

//这里考虑一个问题：全局模板特化可能会生成一个与泛型模板实例化生成的版本，这里的解决办法就是不让这两者出现在同一个文件中
template <typename T>
class Invalid{};

Invalid<double> x1;//产生一个实例化体:

//template <>
//class Invalid<double>;//错误，报错：error C2908: 显式专用化；已实例化“Invalid<T>”

//tips:但是当这样的情况出现在两个翻译单元中的时候就不容易被发现，这时候比较可行的办法就是将特化定义放在泛型定义之后，放在一起这样就不会出现不易被发现的情况

//由于没有把类模板定义放在一个干净的文件中，并在后面附加特化版本的声明，在一个任意的其他文件中定义类特化版本，
//下面使用的时候就会碰到头文件包含顺序的问题，还有可能有使用泛型定义的可能性

#include "template_class2.h"//这个文件必须放在template_class.h的前面，如果不包含这个文件得到的将会是泛型的定义
#include "template_class.h"


void testT8_5(){
	testBuffer();
	//clear(buffer);
	//输出： Danger<void>::max = 100
}

//看下面这个正确的例子，这时候特化的定义，头文件的包含就可以随意了(下面使用CHOICE作为开关，两种情况都可以)
#define CHOICE

#ifdef CHOICE

	template <typename T>
	class Correct;

	template <>
	class Correct<void>{//特化
	public:
		enum {value = 28};
	};
	#include "template_correct_class.h"

#else

	#include "template_correct_class.h"

	//头文件包含在前，就不需要有下面的模板声明了
	//template <typename T>
	//class Correct;

	template <>
	class Correct<void>{//特化
	public:
		enum {value = 28};
	};

#endif


void testT8_5_0(){
	cout << "Correct<void>::value : " << endl;
	cout << Correct<void>::value << endl;
	cout << Correct<int>::value << endl;
	//输出：
	/*
	Correct<void>::value :
	28
	14
	*/
}

//如果遇到使用的泛型定义不是有自己所写的时候：
//可以将给定的头文件用一个自己的头文件包起来，后面写上自己的特化版本，通过这种包装使用上面的机制

//下面看全局的函数模板特化
//相比较类模板的特化，函数模板特化新加了两个概念：重载和实参演绎
//比如说下面的这个函数模板的特化
template <typename T>
int f4(T){
	return 1;
}
template <typename T>
int f4(T* ){
	return 2;
}
//f4(T)的特化
template <>
int f4(int){
	return 3;
}
//f4(T*)的特化
template <>
int f4(int*){
	return 4;
}
//全局函数模板特化不允许包含缺省实参值，但是特化版本的函数模板会使用基本模板的所有缺省实参值
template <typename T>
int f5(T, T x = 42){
	return x;
}
//template <>
//int f5(int, int = 35){//错误，报错：error C2765: “f5”: 函数模板的显式专用化不能有任何默认参数
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
	//输出:  21
	//调用了g(int, int y)这一特化版本，并使用了基本模板的缺省实参
}

//全局特化声明和普通声明很相似，可以把全局特化声明看做是一个普通的再次声明。全局特化声明的声明对象不是一个模板。
//所以有了类似类模板特化遇到的相同的问题，非内联的全局函数模板特化只能定义一次，并且要避免由模板直接生成特化版本的函数。
//具体做法有两个：
//一种做法是：将特化版本的“声明”放在紧跟基本模板后面，放在一个文件A中，并将特化版本的定义放在另一个文件B中
//            如果将定义也直接放在基本模板定义的文件中，那么必然是放在基本模板定义之后，并且这时候特化版本的函数不被用到
#include "template_fn.h"//这个头文件中有 fn的模板和fn的特化声明

void print(){
	fn(1,2);//如果template_fn.h中没有fn特化的声明，那么这行就会引起一个错误：
	        //error C2908: 显式专用化；已实例化“int fn<int>(T,T)”
}

//下面是fn的特化版本定义
template<>
int fn(int, int y){
	return y/2;
}

void testT10(){
	cout << fn(0) << endl;
	//输出14
}

//另一种办法是将定义放在基本模板，但是必须将其声明为inline函数
#include "template_fn2.h"  //该文件中定义了fn2的基本模板和内联版本的函数特化定义

void testT11(){
	cout << fn2(0, 30) << endl;
	//输出： 15
}

//下面看 全局成员特化
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
int Outer<T>::outer_value = 6;//泛型版本的赋值

template <typename T>
template <typename U>
int Outer<T>::Inner<U>::inner_value = 7;//泛型版本的内部类的静态成员赋值，需要写两层template前缀

//下面是Outer类bool型特化
template <>
class Outer<bool>{
public:
	template<typename U>
	class Inner{
	private:
		static int Inner_value;
	};
	void print()const{//特化版本的成员函数重写
		cout << "specific" << endl;
	}
};
//接下来是Outer类成员的全局特化
template <>
int Outer<void>::outer_value = 12;
//有这里可以看出，对于普通类的成员函数和静态变量而言，类外声明在C++中是不允许的，但是对于类模板特化成员，这是合法的
template <>
void Outer<void>::print()const{
	cout << "Outer<void>" << endl;
}
//下面成员特化的声明类似于定义，但是这些声明仍被视为声明(为避免重复就注释掉了)
//template <>
//int Outer<void>::outer_value;
//template <>
//void Outer<void>::print()const;

//如果想要对Outer<T>::Inner做成员特化，可以这样：
template <>
template<typename X>
class Outer<wchar_t>::Inner{
public:
	static long inner_value;//Outer<wchar_t>版本成员特化，Inner中的inner_value变成了long类型
};

template <>
template <typename X>
long Outer<wchar_t>::Inner<X>::inner_value;
//外部类特化，内部类仍是模板，这就是外部类的全局成员特化

//下面来看看内部类的特化，当然内部类的特化只能是外部类的某一种特化时的特化
template <>
template <>
class Outer<char>::Inner<wchar_t>{
public:
	enum {inn = 1};
};

//下面的写法是错误的：
//template <typename X>
//template <>
//class Outer<X>::Inner<void>;//报错：error C3212: “Outer<T>::Inner<void>”: 模板成员的显式专用化必须是显式专用化的成员

//对于已经特化过的Outer<bool>类型的内部类特化可以这样写：
template <>         //只需要一个template<>前缀，这再次体现了我们可以认为特化过的类可以视为普通类
class Outer<bool>::Inner<wchar_t>{
public:
	enum {inn = 2};
};

//下面的写法现在也是合法的，
class DefaultInitOnly{
public:
	DefaultInitOnly(){}
private:
	DefaultInitOnly(DefaultInitOnly const&);//不存在拷贝操作
};

template <typename T>
class Statics{
private:
	static T sm;
};
//为Statics的DefaultInitOnly类型提供一个全局特化定义
DefaultInitOnly obj;
template <>
DefaultInitOnly Statics<DefaultInitOnly>::sm;

//下面看局部的类模板特化
//这个情况发生在有的时候，我们并不是想把整个类模板设计成一个针对具体实参列表的全局特化，
//而是希望把类模板特化成一个针对模板实参的类家族。或者简单地说，我们想让模板针对某一类型数据共享一个实现
template <typename T>
class List{
public:
	//.....
	void append(T const&);
	inline size_t lenght()const;
	//.....
};

//如果一个工程里使用了大量的不同类型的List，那么这将会实例化出很多新的代码，从而加大程序的体积。但是仔细看的话，比如append
//对于同样是指针类型的append，其操作都是完全相同的，就是说我们希望有一个指针类型List共享同一个实现。
//那么我们可以特化一个关于指针类型的List。

//下面这段是根据后文所描述的，插在这里的
template<>
class List<void*>{
	void append(void* p);
	inline size_t length()const;
};


template <typename T>
class List<T*>{//新的特点在这里<T*>
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
//因为这个模板的实参只是被部分使用，所以被称作局部特化
//局部特化的语法是，前面是一个template<...>参数化列表，在类模板的名称后面显式指定模板的实参列表，比如这里的<T*>
//但是根据第11章所讲的参数演绎的时候，不能有潜在的无穷演绎存在，而List<void*>会引起List<void*>的演绎，这样就存在问题。所以这里应该先定义一个void*的全局特化
//template<>
//class List<void*>{
//	void append(void* p);
//	inline size_t length()const;
//};//这段代码需要出现在上面的局部特化前面

//这样可以消除演绎递归的问题

//下面是一些局部特化的约束：
//1. 类型和非类型的匹配
//2. 局部特化参数列表不能有缺省实参，局部特化仍会使用基本类模板的缺省实参
//3. 局部特化的非类型实参只能是非类型值，而不能有依赖型表达式
//4. 局部特化的模板实参列表不能喝基本类模板参数列表完全相同，这一条在旧的编译器上是约束，但是在新的编译器上没有这个约束
template <typename T, int I = 3>
class S4;

//template <typename T>
//class S4<int, T>;          //错误，违反约束1  报错：error C2975: “S4”:“I”的模板参数无效，应为编译时常量表达式

//template <typename T = int>
//class S4<T, 10>;            //错误，违反约束2   报错：error C2756: “S4<T,10>”: 部分专用化中不允许有默认参数

//template <int I>
//class S4<int, I * 2>;       //错误，违反约束3   报错：error C2755: “S4<int,I*2>”: 部分专用化的非类型参数必须是简单标识符

template <typename U, int K>
class S4<U, K>;             //vc2010上正确，但是在gcc上因为违反约束4而报错：部分特例化‘S<U, K>’并未特例化任何模板参数

//局部模板也会和基本模板发生关联，当使用一个模板的时候，编译器会对基本模板进行查找，
//但是接下来会匹配调用实参和相关特化的实参。最后会根据和重载函数模板选择原则一样来选择一个最特殊的特化。如果有多个一样的特化就会产生二义性

//最后要指出的是，类模板局部特化的参数个数可以和基本模板不一样。
//下面再演示一种成员指针类型的特化
template <typename C>
class List<void *  C::*>{//针对指向成员指针的指针
public:
	//指向void*的成员指针的特化，为了后面的任意类型的演绎合法而存在
	typedef void * C::* ElementType;
	//...
	void append(ElementType pm);
	inline size_t length()const;
};

template <typename T, typename C>  //注意，这里的参数个数和上面的不一样
class List<T* C::*>{//局部特化
private:
	List<void* C::*> impl;
public:
	//针对任何指向成员指针的指针类型(T*)的局部特化
	//void*类型的成员指针类型在前面已经处理
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