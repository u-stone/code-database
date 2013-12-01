#ifndef _EXAMPLE_H
#define _EXAMPLE_H

#include <iostream>
using namespace  std;

/*模板中的名称*/

//名称查找
//在正式学习之前先看几个名词解释，这个后面会用到，如果迷惑了，可以返回来看这里：
//受限id(qulified-id)：用类名或者命名空间最为前缀的标示符，包括普通标示符、运算符id(+ - * new [] 等可以位于operator之后进行重载的运算符号)、类型转换id（用来定义隐式类型转换的运算符）、模板标示符
//受限名称(qulified name)：在受限id前面加上成员访问符（.或者->）的非受限id。比如：S::x，this->f,p->A::m，但是如果成员访问符不是显式给出的，而是依赖上下文的，那就不算是受限名称，比如前面说的this->f写成f，就不是受限名称
//依赖型名称(dependent name)：一个（以某种方式）依赖于模板参数的名称。比如:
//                          1. 显式包含模板参数T的受限和非受限名称；
//                          2. 用成员访问运算符(.或者->)限定的受限名称，并且这个名称左边的表达式类型是依赖于模板参数T的；
//                          3. 模板中出现的this->b中的b；
//                          4. 函数调用ident(x, y, z)中有某个参数是依赖于模板的参数，那么ident也是依赖型名称，有点像是“连坐”

//先看非模板的
int x;
class B{
public:
	int i;
};
class D : public B{
};

void testT1(D* pd){
	if (pd)
		pd->i = 3;   //正确
	//D::x = 2;  //错误，很显然，我们都知道D里面是没有x的，尽管全局作用域里面有x，也是不可以的，报错：error C2039: “x”: 不是“D”的成员
	//在这里我们得到的规则是，类作用域的名称查找包括其基类，但是不会跨出类继承体系跑到外围作用域来；注意这是受限作用域的名称查找
}

extern int count;              //(1)
int lookup_example(int count){ //(2)
	if (count < 0){
		int count = 1;         //(3)
		lookup_example(count);//非受限的count，这里会引用(3)
	}
	return count +            //非受限的count，这里引用的是(2)
		::count;              //受限的count，这里引用的是(1)
}
//以上内容可以用以前简单、直观的知识读懂，下面看C++引入模板之后，非受限名称发生了什么

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
	BigNumber x = max__(a, b);//max__实例化的时候是不知道BigNumber的定义的，但是这里可以正确执行，
	                          //因为C++引入一个概念叫做Argument Dependent Lookup（ADL）依赖于参数的查找
	                          //也就是ADL会打乱原来常见的顺序名称查找
}

//下面看ADL的细节

//ADL会查找参数相关的namespace，和class相关的作用域
template <typename T>
class C{
	friend void f();
	friend void f(C<T> const &);//正确，gcc中提示：警告：友元声明‘void f(const C<T>&)’声明了一个非模板函数
};

void testT2(C<int>* p){
	//f();    //错误，f()没有参数和返回值类型的关联类或者关联命名空间，所以这个函数不会找到类C中，因而这是一个无效的函数调用。报错：error C2780: “void f(const C<T> &)”: 应输入 1 个参数，却提供了 0 个
	f(*p);  //根据ADL原则，因为f(*p)中的参数包含了类型C，这样就可以在类C中找到友元函数的声明，找到了名字，就可以通过编译了。
}
//类C的友元函数的一个特化
void f(C<int> const&){
}

void f(){//注意上面testT2()中的f()找不到这里，如果这个前置了，那么是可以找到的，但这样的话testT2()中的f()就跟类C中的友元函数没什么关系了

}

//接下来看看“插入式类名称”，就是在类的作用域中插入了该类的名称，相应的模板也有类似的情况

//先看非模板类的插入式类名称
int E;

class E{
private:
	int i[2];
public:
	static int fsize(){
		return sizeof(E);//有插入式类名称E
	}
};

int fsize(){
	return sizeof(E);
}

void testT3(){
	cout << "E::fsize() = " << E::fsize() << ", "//这里E调用的是类型E的sizeof
		<< " ::fsize() = " << ::fsize() << endl; //这里E调用的是int型sizeof
	//输出结果：E::fsize() = 8,  ::fsize() = 4
}

//再看模板类型的插入式类名称
template <template <typename T> class TT>
class X{

};

template <typename T>
class CY{
	CY* a;        //类名称CY后面虽然没有模板参数，但是也会被当做CY<T>* a;
	CY<void> b;   //类名称CY后面有模板实参，那就是插入式类模板名称
	//以上二者都是插入式类模板名称

	//X<CY> c;      //错误，CY后面没有模板参数，是不会被看作模板名称的，报错:error C3200: “CY<T>”: 模板参数“TT”的模板参数无效，应输入类模板
	X<::CY> d;    //正确，CY前面加上了域作用符::成为受限名称，会被认为是模板名称的；但在老的编译器上这个是通不过编译的，因为<:就是[，但是新的编译器可以识别了
	X< ::CY> e;   //这是为了兼容老的编译器的写法
};

//////////////////////////////////////////////////////////////////////////
//下面看一下依赖类型名称，这个是由于C++作为一门上下文相关语言而来的
//举个例子，X<1>(0)的含义就取决于X到底是个啥，如果是一个模板也可以；是一个变量也可以，解释为(x<1)>0
//那么在模板中，我们看一个由于模板的显式特化而造成的类型判断问题
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
		Trap<T>::x*y;//VS2010中把这个认为是Trap<T>::x类型的指针，而没有把这个认为是Trap<T>::x与y的积
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
//C++语言规定(可能有变化)：通常而言，依赖型受限名称并不会代表一个类型，除非在这个名称之前有关键字typename。
//这里的规则是，当遇到以下情形时，必须使用typename（使用class是不可以的）
//(a)名称出现在模板中，就是说在非模板定义中出现，是会报错的（相当于废话）
//(b)名称是受限的(参考文前概念解释)
//(c)名称不是用于指定基类继承列表中，也不是位于构造函数的成员初始化列表；这段话设定的条件和上面说的前提一肯定一否定，让人容易头晕。注意这里的意思是如果在继承列表和初始化列表中不能使用typename
//(d)名称依赖于模板参数
//(e)只有在(a)、(b)、(c)三个条件同时满足的时候才能使用typename
template <typename T>               //(1) 这是正常的模板参数前缀
struct S : /*typename*/ XX<T>::Base{//(2) 错误，违反规则(c)，报错：error C2518: 关键字“typename”在基类列表中非法；被忽略，在gcc中报错：错误：expected class-name before ‘XX’

	S() : /*typename*/ XX<T>::Base( //(3) 这里我试了一下，有没有typename都是可以的，并不是规则里说的不能有typename
		/*typename*/ XX<T>::Base(0))//(4) 同上，这可能是C++的规则(c)有变化，
	{
	}
	typename XX<T> f(){             //(5) 有没有typename都可以，
		typename XX<T>::C *p;       //(6) 有没有typename都可以，
		X<T>::D* q;                 
	}
	typename XX<int>::C * s;        //(7) 有没有typename都可以
};

struct U{
	//typename XX<int>::C * pc;     //(8) 错误，违反规则(a)，报错：  error C2899: 不能在模板声明之外使用类型名称，
};

void testTII(){
	//S<int> s;        //如果加上这行代码，那么上面很多行代码都会报错，这里是要说，实际上上面有很多代码并没有实例化，而是被编译器的延迟编译绕过了
}

//下面附一个gcc中的编译信息
template <typename T>                //(1) 这是正常的模板参数前缀
struct S2 : /*typename*/ XX<T>::Base{//(2) 报错：错误：expected class-name before ‘XX’
	S2() : /*typename*/ XX<T>::Base( //(3) 错误：关键字‘typename’不允许用在此上下文中(限定的初始值设定隐式地是一个类型)
		/*typename*/ XX<T>::Base(0)){//(4) 
	}
	/*typename*/ XX<T> f(){         //(5) 报错：错误：expected nested-name-specifier
		typename XX<T>::C *p;       //(6) 正确
		//X<T>::D* q;               // 报错：错误：‘q’在此作用域中尚未声明，如果前面加上typename前缀就可以了
	}
	typename XX<int>::C * s;        //(7) 有没有typename都可以
};

struct U2{
	//typename XX<int>::C * pc;     //(8) 报错：  错误：在模板外使用‘typename’  这里是违反了规则(a)
};

//////////////////////////////////////////////////////////////////////////
//依赖型模板名称 
template <typename T>
class Shell{
public:
	template <int N>
	class In{//嵌套类
	public:
		template <int m>
		class Deep{//嵌套类中的嵌套类
		public:
			virtual void f();
		};
	};
};

template <typename T, int N>
class Weird{
public:
	//以下使用了嵌套类来对依赖型名称说事儿
	void case1(typename Shell<T>::template In<N>::template Deep<N>* p){//正确
		p->template Deep<N>::f();//注意这是虚函数
	}
	void case2(typename Shell<T>::template In<N>::template Deep<N>& p){//正确
		p.template Deep<N>::f();
	}
	void case3(typename Shell<T>::In<N>::Deep<N>* p){//正确，不加template关键字也可以，旧的编译器上可能不行
		p->Deep<N>::f();
	}
	//void case4(Shell<T>::In<N>::Deep<N>* p){//错误，不加typename是不合法的，报错：error C2061: 语法错误: 标识符“Deep”
	// p->Deep<N>::f();
	//}
	//void case5(Shell<T>::In<N>::Deep<N>& p){//错误，不加typename是不合法的，报错：error C2061: 语法错误: 标识符“Deep”
	// p.template Deep<N>::f();
	//}
	//指针类型总是和引用类型情况相同的
};
//结论
//也就是说如果想要用一个较深的嵌套来说明类型，那么可以省去template关键字，但是最前面必须加上typename关键字，这应该是新的C++编译器对模板的支持
//这一点上，在旧的编译器上template是不可以省略的。

//接着看使用了using之后的效果

//使用using可以引出类和命名空间的名称
class BX{
public:
	void f(int);
	void f(char const *);
	void g();
};
class DX : private BX{
public:
	using BX::f;//这样在DX中就可以使用BX中的f了，这里并没有指明是哪一个f。但是这种写法破坏了C++的访问级别声明，将来可能不会包含这个机制
	//BX::f;  //这种机制可能会取代上面的，现在只能使用一种
};
//但是上面这样写并不能说明引入的类型到底是类名称、模板名称、还是其他名称
template <typename T>
class BXT{
public:
	typedef T Mystery;   //定义一种类型
	template <typename U>
	struct Magic;
};

//引入类型
template <typename T>
class DXTT : private BXT<T>{
public:
	using typename BXT<T>::Mystery;//有或者没有这行，下面都是正确的
	Mystery * p;                    //正确
};

//引入模板类
template <typename T>
class DXTM : private BXT<T> {
public:
	using BXT<T>::/*template*/ Magic;//有或者没有这行，下面都是正确的
	Magic<T>* plink;                   //正确
};

//ADL和显式模板实参
namespace N{
	class X{
	};
	template <int I>
	void select(X* );
};
void gnx(N::X* xp){
	using namespace N;  //如果没有这行引入命名空间N，后面的就是错的（报错：error C2065: “select”: 未声明的标识符），如果有这行，下面的就可以找到select()。这里是说下面一行并不能根据ADL找到select标识符，这二者是平行的
	//N::select;     //这样声明是不行的，看来这和using的效果还是不一样
	select<3>(xp);   //取决于using namespace N;一行
}
void N::select<3>(X* ){//select的定义

}

//派生类和类模板
template <typename X>
class Base{
public:
	int basefield;             //注意，basefield是一个非依赖型名称
	typedef int T;             //这里会和后文的用法一起产生一个反直观的语义
};

class D1 : public Base<Base<void>>{//非模板，这里的基类类型是非依赖名称，基类是完全确定的，不依赖与模板参数
public:
	void f(){
		basefield = 3;         //注意，basefield是一个非依赖型名称
	}
};
template <typename T>
class D2 : public Base<double>{
public:
	void f(){
		basefield = 7;
	}
	T strange;                 //注意这里的T不是模板参数中的T，而是非依赖类型基类中的typedef之后的T，这一点是不直观的
};

void gD2(D2<int*>& d2, int* p){
	//d2.strange = p;           //错误，报错：error C2440: “=”: 无法从“int *”转换为“Base<X>::T”
	              //这段需要仔细看，d2.strange被视为Base<double>::T类型（在这个例子中是int，就是typdef那句定义的T），而不是D2定义中的模板参数类型(这个例子中是int*)。
	              //因为对于基类(这里的Base<Base<void>>)是非依赖类型的时候，它的派生类中查找一个非受限名称，会先查找这个基类，然后才是模板参数类表
}

//C++中规定，对于模板中的非依赖型名称，将会在看到的第一时间进行名称查找，而不是在实例化的时候才查找
//           对于非依赖类型名称不会在依赖类型基类中查找。
//     但是，对于依赖类型名称，只有在实例化的时候才会进行查找。这时候，基类也就是特化的了，即非依赖类型。
template <typename T>
class DD : public Base<T>{//这儿Base<T>就是依赖类型基类了
public:
	void f(){
		//basefield = 0;//(position 1)，错误，原因见后文，报错：
		/*
		error C2440: “=”: 无法从“int”转换为“”
		转换为枚举类型要求显式转换(static_cast、C 样式转换或函数样式转换)
		g:\visual studio 2010\projects\cpptemplates\cpptemplates\chap09\example.h(326): 编译类 模板 成员函数“void DD<T>::f(void)”时
		with
		[
		T=bool
		]
		g:\visual studio 2010\projects\cpptemplates\cpptemplates\chap09\example.h(335): 参见对正在编译的类 模板 实例化“DD<T>”的引用
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
	d.f();//(position 3) 上面的错误，由这里引起
}
//上面的（position 1）处，编译器发现basefield是一个非受限名称，那么立即进行名称查找，并在Base中找到，即int型变量。如果没有(position 3)，这里是没有问题的
//但是在（position 3）处，d作为一个显式实例化的对象，会先找Base<bool>特化定义，这里又将basefield定义为了enum类型常量。因此就在上面报错，说无法进行类型转化
//为了解决上面的问题，我们可以让f()函数中的basefield变成依赖型名称，这样就可以进行延迟查找了，比如写作：
//                （1）this->basefield = 0;     或者
//                （2）通过受限名称引入依赖型：Base<T>::basefield = 0;
//方法（1）
template <typename T>
class DD1 : public Base<T>{
public:
	void f(){
		this->basefield = 0;
	}
};
//方法（2）
template <typename T>
class DD2 : public Base <T> {
public:
	void f(){
		Base<T>::basefield = 0;
	}
};

//再来看一种情形，说明第一种方案比第二种方案靠谱(但是在新的编译器上好像差不多)
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
	pddt->f();//正确，输出：
	/*
	DDT<T>::zero()
	DDT<T>::one()
	*/
	delete pddt;
	DT<int> * pdt = new DT<int>;
	pdt->f();//正确，输出：
	/*
	DT<T>::zero()
	DT<T>::one()
	*/
	delete pdt;
}
//上面的语法没有问题，输出也没有问题，看来新的C++编译器并没有禁用zero()，one()的虚函数调用，但是在旧的编译器中f()的调用时会报错的，
//////////////////////////////////////////////////////////////////////////

#endif