#ifndef EXAMPLE_CHAP11_H
#define EXAMPLE_CHAP11_H

#include <iostream>
using namespace std;

//模板实参演绎
//本章讲解的是关于模板的参数演绎方面的规则
//这里有几个首要的规则是：
//1. 实参-形参对的分析是独立的
//2. 即使参数匹配成功也不一定可以演绎成功
//3. 演绎的时候在某些情况下会发生decay转型：数组和函数指针会隐式转为指针类型，并且这个时候会忽略const和volatile限定符
//4. 但是对于引用类型是不会发生decay的

template <typename T>
T const& max_(T const& a, T const & b){
	return a<b?b:a;
}

void testT1(){
	//int g = max_(1, 1.0);//报错：error C2782: “const T &max(const T &,const T &)”: 模板 参数“T”不明确
	double g2 = max_<double>(1, 1.0);//正确，但是警告：warning C4244: “初始化”: 从“const double”转换到“int”，可能丢失数据
}
//原因分析：根据独立演绎下来的结果，如果按照第一个参数，T应该是int，但是根据第二个参数演绎下来的结果是double，这时候两个产生了矛盾：一个参数T有了两个可能的值。于是就报错了

template<typename T>
typename T::Element at(T const& a, int i){//这里的typename必须加上
	return a[i];
}

void f(int* p){
	//int x = at(p, 7);//错误，因为T为int*，那么不存在(int*)::Element这个构造，于是就失败了。报错：error C2893: 未能使函数模板“T::Element at(const T &,int)”专用化
	//int x2 = at<int*>(p, 7);//错误，报错：error C2770: “T::Element at(const T &,int)”的显式 模板 参数无效
}

template <typename T>
void f(T){}

template <typename T>
void g(T&){}

double x[20];
int const seven = 7;

void testT2(){
	f(x);     //正确，T是double*，这里发生了decay
	g(x);     //正确，引用类型，T是double[20]，引用类型不会发生decay的
	f(seven); //正确，T是int
	g(seven); //正确，引用类型，T是int const
	f(7);     //正确，T是int
	//g(7);     //错误，不能把7传递给int&。报错：error C2664: “g”: 不能将参数 1 从“int”转换为“int &”
}

//关于引用类型不会发生decay可以看一个例子
template <typename T>
T const& max1(T const& a, T const& b){
	return a;//这里没有做比较
}
template <typename T>
T const max2(T const a, T const b){
	return a;//这里没有做比较
}
void testT3(){
	//max1("apple", "pear");//错误，报错：
	/*
	error C2782: “const T &max_(const T &,const T &)”: 模板 参数“T”不明确
	g:\visual studio 2010\projects\cpptemplates\cpptemplates\chap11\example.h(53) : 参见“max_”的声明
	可能是“const char [5]”
	或    “const char [6]”

	由此可见apple对应const char[6]，pear对应const char[5]
	*/
	max2("apple", "pear");//发生了decay，二者都变成了char const*
}

//对于复杂类型的参数，匹配过程是从最顶层开始不断递归进行匹配的，下面来理解一下“顶层”和“递归”的含义
template <typename T>
void f1(T* ){}

template <typename E, int N>
void f2(E(&)[N]){}

template <typename T1, typename T2, typename T3>
void f3(T1 (T2::*)(T3*)){}//这个有点小复杂，这里的类型是T2类型中的一个成员函数指针，返回值是T1，参数只有一个，是T3*

class S{
public:
	void f(double*){}
};

void testT4(){
	int *** ppp = NULL;
	bool b[42];
	f1(ppp);   //T是int**
	f2(b);     //T是bool，N是42
	f3(&S::f); //T1=void, T2=S,T3=double
}
//由于演绎的递归性质，（递归必须是有尽头的），有些构造的是不能作为演绎上下文的：
//1. 某些受限类型的名称，比如Q<T>::X不能代替T，否则T->Q<T>::X，Q<T>::X->Q<Q<T>::X>::X，接着展开……无尽头
//2. 除了非类型的参数之外，模板参数中海油其他成分的非类型表达式，比如：S<I+2>就不能代替I，否则I->S<I+2>，S<I+2>->S<S<I+2>+2>，也是无尽头。
//3. 同2，还有比如：int(&)[sizeof(T)]不能代替T，道理同上
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
	//fppm(&X<33>::f);//按照书上说的意思N是被转换为33，但是VS2010解析不了如此复杂的模板，报错：error C2893: 未能使函数模板“void fppm(void (__thiscall X<N>::* )(X<N>::I))”专用化
	//说明：按照书上说的思路，演绎中发生了这样的情况，X<N>::I是不可演绎的上下文，但是X<N>::*p的成员部分X<N>是可以演绎的，
	//      于是就得到了参数N，而后把N放入不可演绎的X<N>::I中，就得到一个和实参&X::f匹配的类型，最后这个实参-形参演绎就成功了
	//      这里作者是要说明，不能演绎的参数类型是可以绕过的，因为演绎的过程不是唯一的，而是多路独立演绎，最后共享演绎结果，只要匹配成功就可以的，这样就有了演绎上下文。
}

//但是，有时候参数类型完全依靠演绎上下文的演绎过程也会出错，见下面的例子：
template<typename T1, typename T2>
class XX{
};
template <typename T>
class Y{};

template <typename T>
void f3(XX<Y<T>, Y<T>>){
	
}
void testT6(){
	f3(XX<Y<int>, Y<int>>());  //正确
	//f3(XX<Y<int>, Y<char>>()); //错误，报错：
	/*
	error C2782: “void f3(XX<Y<T>,Y<T>>)”: 模板 参数“T”不明确
	g:\visual studio 2010\projects\cpptemplates\cpptemplates\chap11\example.h(124) : 参见“f3”的声明
	可能是“char”
	或       “int”

	接着还报错：
	error C2784: “void f3(T1 (__thiscall T2::* )(T3 *))”: 未能从“XX<T1,T2>”为“T1 (__thiscall T2::* )(T3 *)”推导 模板 参数
	with
	[
	    T1=Y<int>,
	    T2=Y<char>
	]

	这里的问题就是:针对f3()的两个参数，演绎出来不同的实参类型，两个X不同类型的临时变量。
	*/
}


//下面看两种特殊的演绎，说特殊只是因为他们发生演绎的时候不是由于函数调用
template <typename T>
void f4(T, T){}
void testT7(){
	void (*pf)(char, char) = &f4;//在这里，模板实参是void(char,char)，形参是void(T,T)，于是char替换T。
}

class S2{
public:
	//template<typename T, int N>
	//operator T[N]&();//本来后文有话要说的，可惜编译器不支持这种写法。这里只做意图的说明吧！
	                   //标准和实现之间的关系，有时候类似理想和现实的关系，说得到不一定做得到。
	void f(int){}
};
void f5(int(&)[20]);
void testT8(){
	S2 s;
	//f5(s);//本来这里想说的是S2可以转型为int(&)[20]，那么在f5()的参数演绎中就可以用int来替换T,用20替换N。
}

//当找不到精确的实参-形参匹配的时候，编译器会考虑接受下面几种宽松的匹配
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
	f5(s1, s2);//当模板形参是引用类型的时候，实参可以不用考虑形参的const，volatile限定符
	//输出：f5() const class S2   class S2
	//如果解开上面注释掉的非const参数类型的f5()函数时，就会发生精确匹配
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
	f6(ps2,2);//会忽略const

	volatile S2 *ps3 = NULL;
	f6(ps3,3);

	S2 *volatile ps4 = NULL;
	f6(ps4,4);//会忽略volatile

	void (__thiscall S2::*p)(int) = &S2::f;
	f6(p,5);
	/*
	输出：
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
	//说明：当实参类型是指针或者成员指针的时候，参数匹配过程中，编译器可以选择忽略实参中的const、volatile限定符进行参数演绎
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
	//f12(&chd);//错误：报错：
	/*
	error C2784: “void f12(B<T>)”: 未能从“Chaos<T> *”为“B<T>”推导 模板 参数
	with
	[
	    T=double
	]
	g:\visual studio 2010\projects\cpptemplates\cpptemplates\chap11\example.h(258) : 参见“f12”的声明
	*/

	//发生向基类的转化，这个发生的时机是：
	//1. 演绎过程不涉及到转型运算符模板时，形参可以是实参的基类类型；或者
	//2. 当实参是指针类型的时，形参可以是实参指向类型的基类

	//输出：
	//in f11(B<T>*) : class B<long>
	//in f12(B<T>*) : class B<long>
}

//上面谈到的全都是函数或者是成员函数的参数演绎，下面介绍关于类模板的参数演绎，只有一条：
// 参数演绎不能用于类模板，并且类模板的构造函数也不能根据实参来演绎类模板参数
template <typename T>
class S3{
public:
	S3(T b) : a(b){}
private:
	T a;
};

void testT14(){
	//S3 s(12);//错误，报错：error C2955: “S3”: 使用类 模板 需要 模板 参数列表
	         //不能为类模板进行参数演绎

	S3<int> s3(12);//正确，显示实例化
}

//关于缺省函数调用实参
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
	init(&s, S4(1,2));//正确，虽然S4没有默认构造函数（所以T()就无效了），但是这里编译的时候直接使用了显示给出的参数，
}
//同样，类模板不能使用参数演绎，对于缺省参数的调用有如下的影响
template <typename T>
void f15(T x = 28){
}
void testT16(){
	f15<int>(); //T=int
	//f15();      //错误，不能根据28来推导T，报错：error C2783: “void f15(T)”: 未能为“T”推导 模板 参数
}

//最后看一下友元函数的定义（本来原书说的是Barton-Nackman方法，但是作为一个过时的技术，作者说是在这里要研究“技术”，我没明白作者的意图）
class S5{};

template <typename T>
class Wrapper{
private:
	T object;
public:
	Wrapper(T obj) : object(obj){}//存在一个隐式转换T到Wrapper的
	friend void f16(Wrapper<T> const & a){
		cout << "In f16() " << typeid(a).name() << endl;
	}
};
void testT17(){
	S5 s;
	//f16(s);//如果这句放在这里是不能通过编译的，通过ADL并不能在S5的基类和外部作用域内找到f6()的定义，报错：error C3861: “f16”: 找不到标识符
	Wrapper<S5> w(s);
	f16(w);//正确，可以通过w在Wrapper<S5>的作用域中找到f16()的定义
	f16(s);//如果这句放在这里，就可以通过编译，估计这是利用了上一行在模板实例化POI时生成的数据。
	//输出：
	/*
	In f16() class Wrapper<class S5>
	In f16() class Wrapper<class S5>
	*/
}

#endif