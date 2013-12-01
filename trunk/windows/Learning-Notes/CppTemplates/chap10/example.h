#ifndef _EXAMPLE_H_CHAP10_
#define _EXAMPLE_H_CHAP10_

#include <iostream>
using namespace std;

//模板的实例化

//按需实例化 on-demand

//像非模板的用法一样，模板中的前置声明也是可行的
template<typename T>
class C1;

C1<int> * p = 0;

template <typename T>
class C1{
public:
	void f();
};

void g(C1<int>& c){
	//c.f();             //错误，报错：error LNK2001: 无法解析的外部符号 "public: void __thiscall C1<int>::f(void)" (?f@?$C1@H@@QAEXXZ)
}

template <typename T>
class C{
public:
	C(int);
};
void candidate(C<double> const& );//(1)
void candidate(int){  //(2)
	std::cout << "candidate(int)" << std::endl;
}

void testT1(){
	candidate(42);   //
	candidate(4.0);  //警告： warning C4244: “参数”: 从“double”转换到“int”，可能丢失数据
	                 //上面一行居然没有提示错误，而是警告，这里是做了隐式转换
	//输出：
	/*
	candidate(int)
	candidate(int)
	*/
}

//延迟实例化，不太准确的描述就是，只对确实需要的部分实例化。模板实例化的过程中也像普通C++代码一样先进行语法检查和语义检查
template <typename T>
class Safe{

};
template <int N>
class Dange{
public:
	typedef char Block[N];//如果N<=0的话，将会出错
};

template <typename T, int N>
class Tricky{
public:
	virtual ~Tricky(){}
	void no_body_here(Safe<T> = 3);//未报错，编译器假设调用的时候不会使用3作为实参，而如果不使用缺省的实参的话，就不会实例化该实参，故而这里没有报错
	                               //并且后文中没有调用该函数，编译器按照只生成需要的部分的规则，这里虽然没有函数体，但是也没有报错
	void inclass(){
		Dange<N> no_boom_yet;
	}

	void error_not_yet(){
		Dange<0> boom;             //当没有直接调用error函数的时候，这行代码，没有被实例化，故而这个错误也没有被编译器报错。可能在老的C++标准、或者老的编译器上这是一个错误
		                           //这儿所说的错误就是Dange的实参为0，造成数组Block[]为空的错误，C/C++中不能分配常量大小为 0 的数组，但是可以动态分配0个空间的指针
	}
	void unsafe(T (*p)[N]);

	T operator -> ();             //非标准格式的->符号重载，这里也没有报错，编译器假设用户修改了重载解析规则。

	//virtual Safe<T> suspect();//这段代码中只有这一出错误，报错：error LNK2001: 无法解析的外部符号 "public: virtual class Safe<int> __thiscall Tricky<int,0>::suspect(void)" (?suspect@?$Tricky@H$0A@@@UAE?AV?$Safe@H@@XZ)
	                          //gcc中报错类似：undefined reference to `Tricky<int, 0>::suspect()'
	                          //报错分析：当后面的Tricky<int, 0> ok被创建出来的时候，编译器要检查该Tricky类型的相关必要存在且合法的函数，比如说构造函数、析构函数
	                          //这里的构造函数是默认的，析构函数是虚函数、且是用户自定义的；另外编译器要构造整个实例，发现该类型存在虚函数，就会检查虚函数的定义，
	                          //这个时候发现suspect没有完整的定义，于是报错。如果后文中没有构造对象ok，或者该函数不是虚函数，那么这里就不会报错
	struct Nested{
		Dange<N> pfew;
	};
	union{
		int align;
		Safe<T> anonymous;
		Dange<N> dange;      //没报错，老的编译器这行可能会报错
	};
};

void testT2(){
	Tricky<int, 0> ok;
	int *a = new int[0];
	//ok.error();
}

//C++实例化模型
//到这一章才开始解释第二章提到的模板编译的两个阶段。
//第一阶段：模板语法检查，这个阶段会使用ADL
//第二阶段，这次是真正的实例化，开始将模板代码翻译成普通代码，这个阶段也会使用ADL。这个阶段会检查所有模板调用点，并翻译模板代码，
//          翻译的地方在模板调用代码的附近，这个地方又叫POI，即Point-of-instantiation实例化点。
//下面看实例化的细节：

class MyInt{//普通类
public:
	MyInt(int i){}
};
MyInt operator - (MyInt const & b){
	return b;
}
bool operator > (MyInt const&, MyInt const&){
	return false;
}
typedef MyInt Int;

template <typename T>
void f(T i){
	if (i > 0)
		g(-i);
}
//(1)
void g(Int){
	//(2)
	f<Int>(42);//这里有模板的调用，那么会在这行代码附近产生模板的实例化
	//(3)
}
//(4)
//下面分析f<Int>(42)的实例化点POI的位置。
/*
    因为模板代码不能位于函数内，所以不可能出现在位置(2)、(3)。
    位置(1)，和(4)的区别是函数g(Int)是否可见的问题。这时候C++规定：
    对于“非类型特化”的引用代码，POI点放在了包含这个引用的定义或声明之后的最近的名字空间域
	那么，在这里就是位置(4)
	注意这里的类MyInt是普通类，没有类模板的实例化
*/

template <typename T>
class S{
public:
	T m;
};
//(5)
unsigned long h(){
	//(6)
	return (unsigned long)sizeof(S<int>);//出现模板调用，这行代码附近会产生POI
	//(7)
}
//(8)
//下面分析S<int>模板代码在(5)-(8)哪一个位置产生POI
/*
    (6),(7)不能产生POI，因为模板代码不能位于函数内部
    如果模板代码像上面的例子一样产生于位置(8)，那么模板代码执行到(8)的时候才会知道S<int>的大小，这样sizeof(S<int>)的调用就是无效的。
    这时候只能放在位置(5)，C++规定：
    对于产生于“模板的类实例”的引用，POI只能定义在包含这个实例引用的定义或声明之前的最近名字空间域，就是位置(5)
	这里S<int>是类模板的实例化
*/

template <typename T>
class S2{
public:
	typedef int I;
};
//(1)
template <typename T>
void f2(){
	S2<char>::I var1 = 41;
	f<T>(8);
	/*typename*/ S2<T>::I var2 = 42;//typename在老的编译器上必须出现
}
void testT3(){
	f2<double>(); //模板代码调用
}
//(2):(2a)(2b)

//下面分析f2<double>()模板代码POI的产生：
/*
    f2<double>()会引起f2()函数的实例化，但是在该函数内部，又有模板代码、这也需要进行实例化，这样就有了二次POI，
	并且S2<char>是非依赖型的类特化，POI位置在位置(1)；
	S2<T>是依赖型类特化代码，C++规定：
	对于非类型实体（f<double>）这种二次POI和主POI(f2<double>())的位置相同
	对于类型实体（S<double>），二次POI的位置位于主POI（f2<double>()）位置的紧前面。
	那么，f2<double>()和S2<T>::I var2 = 42的POI分别在位置(2b),(2a)
*/

//////////////////////////////////////////////////////////////////////////
//至此，上面所讲的内容都是在同一个文件中的模板编译，下面看一下跨文件编译，跨编译单元，这就需要另一个文件example1.h配合
#include "example1.h"
/*export*/ template <typename T>//这里出警告，但是直接影响后文，所以只能全部注释掉了，后据网上说vc和gcc都不支持分离模型：warning C4237: 目前还不支持“export”关键字，但已保留该关键字供将来使用
	T const& max__(T const&, T const& );

void testT4(){
	std::cout<< max__(7, 42) << std::endl;//(1)
}

//example1.h中的代码，放这里先
// export template <typename T>
// 	T const& max__(T const& a, T const&b){
// 		return a < b ? b : a;//(2)
// }

//同样的原因，vc和gcc都不支持export分离模型，因而只能使用包含模型#include"xxx"
// /*export */template <typename T>
// T const& max__(T const, T const&);

namespace N{
	class I{
	public:
		I(int i):v(i){}
		int v;
	};

	bool operator < (I const& a, I const& b){
		return a.v < b.v;
	}
}

void testT5(){
	std:cout << max__(N::I(7), N::I(42)).v << std::endl;//(3)，错误，报错：error C2668: “max__”: 对重载函数的调用不明确
}

//本来是要看一下跨编译单元编译的时候发生的一些状况，但是vc和gcc都不支持分离模型，这里就只是说一下知识要点：
//上一章提到过的模板名称查询，说编译分为两个步骤，第一个阶段为解析，第二个阶段为实例化，这里要具体说一下各种类型的名称查找：
/*
                     非依赖型名称      非受限依赖型名称     受限依赖型名称
    第一阶段      普通查找、ADL查找        普通查找              NULL
	第二阶段            NULL               ADL查找        普通查找、ADL查找

	这里解释一下上面说的内容，第一阶段发生的是：对于“非依赖型名称”进行普通查找规则和ADL规则的查找；
	       对于“受限的依赖型函数名称”（即函数参数是依赖型的）会使用普通查找规则，这时候会把查找结果保存起来。
	第二阶段发生在POI的时候，这时候会对“受限的依赖型名称”使用普通查找和ADL查找规则进行查找；
	       而非受限的依赖型这时候会接着上次做过的普通查找进行ADL查找，两次查找的结果共同构成一个集合，然后借助重载解析，选出最佳的被调函数
*/

//但是通过下面的例子可以看出，上面说提到的规则好像在新的编译器下有所变化
template <typename T>
void f1(T x){
	g1(x); //(1)
}
void g1(int){

}
void testT6(){
	f1(7);//本来书上说是这里会报g1()找不到定义的，但是这里没错
}

//说到这里，本章后面的模板编译和链接的三种实现方案，就不提了，反正说的是方案设计的问题，我们普通程序员接触到多是某一种实行。
//不过这里需要注意区分几个容易混淆的概念：（显式，隐式）实例化，特化，偏特化，参数演绎，他们分别：
//实例化一般是指模板调用的时候，使用了某一种类型替换方案。是一个模板到普通代码的过程。这包括显式和隐式两种。
//显式实例化对应的是隐式实例化，显示实例化模板max(T,T)的话，就要定义为max<int>(int, int)，这样可以提高编译效率
//特化是说对于某一个模板进行特殊的定义，比如说定义了一个模板函数max(T, T)，为了提高效率又定义了一个int的特化版本max(int ,int)
//偏特化就是对部分参数进行明确指定，要是全部指定了，那就是特化了
//最多用到的就是参数演绎了,就是在使用模板的时候直接赋值参数，然后类型判断就交给编译器，编译器会做一些匹配，最后选择最佳方案
#endif