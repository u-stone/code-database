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

//注意这里的顺序必须是从外围到内部，否则会通不过编译的。
//报错：“List<T>::{ctor}”: 无法将函数定义与现有的声明匹配
template <typename T>
template <typename T2>
List<T>::List(List<T2> const &){//位于类外部的函数定义
	cout << "List Copy Constructor" << endl;
}

template <typename T>
int Length(List<T> const &);

//=========================================================================
class Collection{//外围类
	//内部类2个 Node， Handle
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
class Collection::Handle{//内部类的外部定义（需要加上限定符）
	
};


//=========================================================================
//联合的模板

template <typename T>
union AllocChunk{
	T object;
	unsigned char bytes[sizeof(T)];
};

//=========================================================================
//模板参数中使用缺省值
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
//虚成员函数不能定义为模板，因为虚函数表中需要知道成员函数的个数，但是这要等到实例化完才能知道：

class Dynamic{
public:
	virtual ~Dynamic(){}

// 	template <typename T>
// 	virtual void copy(T);//这个有错误，必须注释掉
};

//=========================================================================
//模板的链接

//首先是名字的链接
int C;
class C;  //类名和非类型变量重名是可以的，变量名和类名不在同一个名字空间

int X;
template <typename T>
class X;   //对于模板来说变量和类名可以重名，在其他版本的编译器上可能不行：（本编译器为Microsoft (R) Message Compiler  Version 1.12.7600）
           /*
		   (GCC) 4.4.7 20120313 (Red Hat 4.4.7-3)上报错：  错误：‘template<class T> struct X’被重新声明为不同意义的符号
		   */

//struct S;  //和下面的有冲突，必须注释掉
template <typename T>
class S;//报错：“S”: 类 模板 已经声明为非类 模板

//再次是名字的链接方式：模板不可以用C链接
extern "C++"   //这是默认的方式，不写也可以
	template <typename T>
void normal();

//extern "C"   //报错： error C2894: 模板不能声明为有“C”链接
	template <typename T>
void invalid();

//extern "Xroma" //有的编译器可能支持，但VC2010不支持，报错： error C2537: “Xroma”: 非法的链接规范
	template <typename T>
void Xroma_link();

//最后是关于名字引用

template <typename T>//这个函数可以直接在别的文件中调用，类似普通函数的用法
void external(){
	cout << "external called external() function" << endl;
}

template <typename T>//这个函数就不能在别的文件中调用，static限制其为本文件内的。那么别的文件中就可以有同名的函数
static void internal();

//模板函数的声明都必须是基本声明，也就是基本模板，都不能在函数名后面加上一对尖括号
template <typename T>
static void basedeclare();

//那就谈一下模板的局部特化，这个概念相对于基本模板
template <typename T1, typename T2>
class MyClass{
	//基本模板
};

//以下全部是局部特化
template <typename T>
class MyClass<T, T>{
	//两个模板参数必须是同类型的
};

template <typename T>
class MyClass<T, int>{
	//第二个参数类型是int
};

template <typename T1, typename T2>
class MyClass<T1*, T2*>{
	//指针类型的模板参数
};

//下面有说明:
template <>
class MyClass<int, int>{//这是类模板的一个特化

};
template <typename T>
class MyClass<T*, T*>{

};

MyClass<int, float> mif;      //MyClass<T1, T2>
MyClass<float, float> miff;   //MyClass<T, T>
MyClass<float, int> mfi;      //MyClass<T, int>
MyClass<int*, float*> mp;     //MyClass<T1*, T2*>

//局部特化的二义性    这参数一多就TMD容易出点岔子
MyClass<int, int>   m1; //同等程度匹配MyClass<T, T> & MyClass<T, int>
MyClass<int*, int*> m2; //同等程度匹配MyClass<T, T>  & MyClass<T1*, T2*>
//这个可以通过添加新的声明来解决：已经添加在上面了
//template <>
//class MyClass<int, int>{
//
//};
// template <typename T>
// class MyClass<T*, T*>{
// 	
// };

//=========================================================================
//模板参数：类型，非类型，模板类型

//模板参数中的类型就相当于是typedef之后的类型，在其后的作用域中不可以重复定义或者声明
template <typename Allocator>
class List_{
	//class Allocator* allocator;//错误:“typename”: 不是“class”
	//friend class Allocator;    //错误: “typename”: 不是“class”
};

//非类型参数, 就是说编译期或链接期可以确定的常值:整型或枚举类型，指针类型，引用类型。
//这类参数只能是右值：不能被取地址或者赋值
template <int buf[5]> class Lexer;
template <int *buf> class Lexer;    //这二者是相同的
//这些参数不能被static mutable修饰，只能被const，volatile修饰

//模板类型作为模板参数
template <
	template <typename T> class C
>
	void f1(C<int>* p);

//但是不可以使用struct和union
//template <
//	template <typename T> struct C//错误： error C2144: 语法错误:“void”的前面应有“;”
//>
//	void f2(C<int>* p);
//
//template <
//	template <typename T> union C//错误，同上
//>
//	void f3(C<int>* p);

class MyAllocator;

template <
	template <typename T, typename A = MyAllocator> class Container
>
class Adaptation{
	Container<int> storage; //使用了缺省的参数Allocator
};

//template <
//	template <typename T, T*> class Buf
//>
//class Lexer{
//	static char storage[5];
//	Buf<char, &(Lexer<Buf>::storage[0])> buf;//这个在VC2010上编译不通过
//};

template <
	template <typename T> class List
>
class Node{
	struct T storage;//这里使用了模板的模板参数T，但是在有的编译器中T是不可以使用，仅作为参数使用
};

//关于模板的模板参数的参数，它的名字一般不会使用，故而常常省略
template <
	template <typename /*T*/, typename /*allocator*/ = MyAllocator>//省略了名字
		class Container
>
class Adaptation_{
	Container<int> storage;//
};

//=========================================================================
//缺省模板参数，任何类型的模板参数都可以有缺省参数

template <typename T1, typename T2, typename T3, typename T4 = char, typename T5 = char>
class Quintuple;

template <typename T1, typename T2, typename T3 = int, typename T4, typename T5>//借用上面的T4 T5的缺省参数
class Quintuple;

//template <typename T1 = char, typename T2, typename T3, typename T4, typename T5>//错误，报错：error C3747: 缺少默认 模板 参数: 参数 2
//class Quintuple;

//缺省参数是不可以重复的
template <typename T = void>
class Value;

//template <typename T = void>//报警告： warning C4348: “Value”: 重定义默认参数 : 参数 1
//class Value;

void cmp(int a, double b = 1.0);
void cmp(int a = 1, double b);//普通的函数声明也可以借用同名函数的缺省参数，同模板参数相同，事实上，这个的b不可以再写成double b = 1.0，否则报错： error C2572: “cmp”: 重定义默认参数 : 参数 2

//=========================================================================
//模板实参:就是模板实例化的时候，用来替换模板参数的值
//分为四种类型：1 显示模板参数，2 注入式类名称，3 缺省模板参数，4 实参演绎

//显示模板参数
template <typename T>
inline T const& max_(T const&a, T const &b){
	return a < b ? b : a;
}
double normal(double a = 1.02){
	return a;
}

void testT1(){//移到main函数中测试
	cout << max_<double>(1.0, -3.0) << endl;//方式1，显示指定 , output: 1
	//?_?
	cout << normal() << endl;
	double tmp;
	cout << (tmp = max_(2.0, -3.0)) << endl;//方式4，实参演绎为double  , output: 1
	//cout << max_<int>(2.0, 3.0) << endl;    //方式1，显示指定int禁止了演绎，返回的是int类型 , output: 3 警告：warning C4244: “参数”: 从“double”转换到“const int”，可能丢失数据
}

template <typename DstT, typename SrcT>//不进行模板实参演绎的要放在模板参数列表的前面，其他的放后面准备进行实参演绎
inline DstT implicit_cast(SrcT const& x){//SrcT可以通过实参演绎，DstT必须通过显示模板参数的方式指定
	return x;
}

template <typename SrcT, typename DstT>//相比上面的例子，这里切换了顺序，影响下面的使用。
inline DstT implicit_cast_2(SrcT const& x){
	return x;
}

void testT2(){
	cout << implicit_cast<double>(-1) << endl;//在这个例子中：DstT被显示指定为double，而SrcT通过实参演绎为了int，也就是说同时发生了显示模板实参和实参演绎
	//cout << implicit_cast_2<double>(-1) << endl;//报错： error C2783: “DstT implicit_cast_2(const SrcT &)”: 未能为“DstT”推导 模板 参数
	cout << implicit_cast_2<int, double>(-1) << endl;//两个模板实参都使用了显示模板实参的方式指定
}

//关于函数模板参数的调用
template <typename Func, typename T>
void apply(Func func_ptr, T x){
	func_ptr(x);
}
template <typename T> void single(T){}

template <typename T> void multi(T){}
template <typename T> void multi(T*){}

void testT3(){
	apply(&single<int>, 3);
	//apply(&multi<int>, 7);//调用multi存在二义性，报错：error C2914: “apply”: 因为函数参数不明确，所以无法推导出 模板 参数
}

//关于模板中“怪异”的SFINAE（substitution-failure-is-not-an-error）：替换失败并非错误
typedef char RT1;
typedef struct {
	char a[2];
}RT2;
RT2	 rt2;

template <typename T> RT1 test(typename T::X const* ){}//如果T中没有X的类型，那么试图创建这样的类型还是不会报错滴
template <typename T> RT2 test(...){ return rt2;}


void testT4(){
	test<int>();//调用的是test(...)，第一个test创建类型失败，
}

//但是如果要试图对替换失败的类型进行计算，那就不可以了，这个不属于SFINAE，关于这个话题$chap15 & chap19$会有更多内容
// template <int I> void f(int (&)[24/(4-I)]);
// template <int I> void f(int (&)[24/(4+I)]);
// 
// void testT5(){//但是这个例子在VC2010 gcc4.4.7上都不支持
// 	&f<4>;//替换后第一个除数为0
// }

//关于不能作为模板实参的类型：1. 未命名的类、枚举类型(这不是废话嘛，都没有名字，还怎么调用呢！)，但是可以通过typedef来改变这一点，但是不同的编译器支持程度不一样；2. 内部类 、内部枚举
class {//匿名类
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
	List<Association*> error1; //vc2010上没问题，gcc上报错：错误：‘template<class T> class List’的模板实参使用了局部类型‘test()::Association*’
	List<ColorPtr> error2;//VC2010上没问题，gcc上报错：错误：‘<anonymous enum>*’是/使用了匿名类型
	List<Point> ok;
}

//还有就是，替换之后必须支持相应的操作才可以
template <typename T>
void clear(T p){
	//*p = 0;//下面有说明
}

void testT7(){
	int a = 0;
	clear(a);//报错：error C2100: 非法的间接寻址
}

//看看下面几种非类型的模板实参
template <typename T, T nontype_param>
class Class;

Class<int, 33>* c1;   //编译期可以确定的整型常量，就是说其他类型常量是不可以做模板参数的
//Class<double, 33.33> * cd;//错误， 报错： error C2993: “double”: 非类型模板参数“nontype_param”的类型非法

int a;
Class<int*, &a>* c2;    //外部变量的地址

void testT8(){
	int a;
	//Class<int*, &a>* c2;    //非外部变量的地址，错误
}

void f();
void f(int);
Class<void (*)(int), f>* c3; //外部函数，这里的地址引用操作符可以省略

void testT9(){
	void f_inner(int);
	Class<void (*)(int), f_inner>* c3; //内部函数，这里的地址引用操作符可以省略
}

int arr[5];
Class<int [5], arr>* c4;   //对于外部变量的数组来说，这里本应该有像上面的a一样的地址引用操作符，这里也可以省略

void testT10(){
	int arr_inner[5];
	//Class<int [5], arr_inner>* c4;   //局部变量的数组 错误，报错：error C2971: “Class”: 模板参数“nontype_param”:“arr_inner”: 局部变量不能用作非类型参数
}

class XClass{
public:
	int n;
	static bool b;
};
Class<bool&, XClass::b>* c5;   //对于static类型的类成员，可以直接使用其自身的类型
Class<int XClass::*, &XClass::n> * c6;//但是对于非static类型的类成员，就必须使用完整的(带 类名::)类型

template <typename T>
void temp_func(){}
Class<void (*)(), &temp_func<double> >* c7;//函数模板实例，同时也是函数

//作为例外，有些常量是不能作为模板实参的：
int *nulptr = nullptr;//空指针
int *const nulptr2 = nullptr;//空指针
//Class<int*, nulptr>* c8; //VS2010上是不可以的，报错：error C2975: “nontype_param”:“Class”的模板参数无效，应为编译时常量表达式
Class<int*, nulptr2>* c08;//常量指针 正确

float flt = 0.0f;
Class<int*, nullptr> * c9;//正确
//Class<float, flt> * c10;//浮点数类型，报错： error C2993: “float”: 非类型模板参数“nontype_param”的类型非法
int inte = 0;
const int con_inte = 0;
//Class<int, inte> * c11;//VS2010上不可以， 报错：error C2975: “nontype_param”:“Class”的模板参数无效，应为编译时常量表达式
Class<int, con_inte> * c12;//正确


char const str[] = "failed string";
extern char const str2[] = "succeed string";
//Class<char const*, str> * c13;//VS2010上报错：error C2970: “Class”: 模板参数“nontype_param”:“str”: 涉及带有内部链接的对象的表达式不能用作非类型参数
Class<char const*, str2> * c14;//成功，这里使用了extern变量，使得str2具有了外部链接

class Base{
public:
	int a;
}base;

class Derived{
}derived;

//Class<Base*, &derived>* err1;//错误， 报错：error C2440: “specialization”: 无法从“Derived *”转换为“Base *”
//Class<int*, &base.a> * err2;//错误， 报错：error C2975: “nontype_param”:“Class”的模板参数无效，应为编译时常量表达式
//Class<int Base::*, &base.a> * err3;//错误， 报错：error C2975: “nontype_param”:“Class”的模板参数无效，应为编译时常量表达式
Class<int Base::*, &Base::a> * suc4;//错误， 报错：error C2975: “nontype_param”:“Class”的模板参数无效，应为编译时常量表达式

int arr_i[10] = {0,1,2,3,4,5,6,7,8,9};
//Class<int*, &arr_i[0]> c15;//错误，error C2975: “nontype_param”:“Class”的模板参数无效，应为编译时常量表达式

//模板的模板参数
#include <list>

template <typename T1, typename T2, 
	template<typename> class Container//这里的类模板形参必须使用class而不是typename，当然也可以是struct, union
>
class Relation1{
public:
private:
	Container<T1> dom1;
	Container<T2> dom2;
};

void testT11(){
	//Relation1<int, double, std::list> rel;//错误，报错：error C3201: 类模板“std::list”的模板参数列表与模板参数“Container”的模板参数列表不匹配
	//因为std::list实际上有两个参数，还有一个缺省的allocator，类模板实参（std::list）必须精确匹配类模板形参：如果形参中没有缺省值，实参不可以带缺省参数，这一点上面的例子看到了
	//         如果形参中有缺省参数，实参中的缺省参数是不被考虑的，就是说会使用形参中的缺省参数，不管你在实参中为缺省参数设置什么值，都不会影响替换时的形参，后面的testT13()证实这一点
}
//修改的方法可以如接下来所示：
template < typename T1,  typename T2, 
	template< typename T, typename = std::allocator<T> > 
    class Container
    >
class Relation2{//模板类
public:
	Container<T1> dom1;
	Container<T2> dom2;
};


void testT12(){
	Relation2<int, double, std::list> rel2;//正确
	//Relation2<int, double, TestList<int, double>> rel3;//错误，报错：error C3201: 类模板“TestList”的模板参数列表与模板参数“Container”的模板参数列表不匹配
}

template < typename T1,  typename T2, 
	template< typename T, typename T3 = char> 
    class Container
>
class Relation3{//模板类
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
class TestList{//模板类
public:
	int size(){
		cout << "get T2 size :" << sizeof(T2) << endl;
		return sizeof(T2);
	}
};

void testT13(){
	Relation3<int, double, TestList> rel;
	cout << rel.size() << endl;//这里会输出1 (就是Container中内部类模板参数缺省值char的size：sizeof(char))，而不是rel中TestList默认参数int得到的sizeof(int)=4
}


//模板实参的等价性

template <typename T, int I>
class Mix{};

typedef int Int;

void testT14(){
	Mix<int, 3*3> * pMix1;
	Mix<Int, 4+5> * pMix2;//pMix1 pMix2是相同的类型，这里typedef定义之后的类型实际比较的还是原来的类型，对于非类型的整型实参比较的是参数的值。
	pMix2 = pMix1;//如果Mix的第二个模板参数类型改为double,float之类，会出现编译错误
}

void testT15(){
	Mix<float, 3.0f> * pMix3;
	Mix<float, 3.8f> * pMix4;
	Mix<float, 4.0f> * pMix5;
	pMix4 = pMix3;//正确，这里的3.8被编译器转成了整型3
	//pMix5 = pMix3;//错误，报错： 
	/*
	error C2440: “=”: 无法从“Mix<T,I> *”转换为“Mix<T,I> *”
	with
	[
	    T=float,
	    I=3
	]
	and
	[
	    T=float,
	    I=4     //注意看这里
	]
	与指向的类型无关；转换要求 reinterpret_cast、C 样式转换或函数样式转换
	*/
}

//友元

//先看友元类
template <typename T>
class TreeNode;

template <typename T>
class Tree{
	//friend class TreeNode;//作为一个模板类，就不存在这种形式了，报错：error C2990: “TreeNode”: 非类 模板 已经声明为类 模板
	friend class TreeNode<T>;//正确
	//friend class TreeNode2<T>; //错误，这时候模板不知道TreeNode2是什么，报错： error C2059: 语法错误:“<”
	friend class unknownClass;//对于非模板类型，这里虽然没有unknowClass的声明，但这是语法允许的。
};

template <typename T>
class TreeNode2;

//友元函数
void multiply(void*);   //普通函数

template <typename T>
void multiply(T);      //模板函数

//普通类
class Comrades{
	friend void multiply(int) {}//合法，这个函数前没有加::，说明这个函数一定不是模板的实例，所以这里还是这个函数的首次声明和定义
	friend void multiply(void*) {}//合法，这是和外部multiply(void)函数不同作用域的定义
	friend void ::multiply(void*);//合法，这个函数前加上了::，说明这是引用前面的一个普通函数或者是模板函数的实例，但是普通函数的优先级比函数模板的优先级高，所以这里是普通函数的引用
	friend void ::multiply(int); //合法，从上面的分析得知，这里是函数模板实例的引用
	//friend void multiply<int>(int){} //错误，虽然函数前没有::，但是后面带有尖括号，说明这是模板函数，要匹配上面的模板函数，但是模板友元函数不能在这里定义。报错：error C3637: “multiply”: 友元函数定义不能是函数 模板 的专用化
	friend void ::multiply<double>(double);//合法，受限的名称还可以具有一对尖括号，但是这个函数必须是可见的
	//friend void invisuable<double>(double);//错误，现在该函数不可见，编译器不知道这是什么类型。对于非模板友元类和非模板函数来说可以不用前置声明而直接引用，但是模板函数不可以，报错：error C2143: 语法错误 : 缺少“;”(在“<”的前面)
	//friend void ::invisuable2<double>(double);//非法，前面又加上了域作用符，说明这是要引用前面的声明或者定义，但是现在不可见，报错：error C2039: “invisuable2”: 不是“`global namespace'”的成员
	friend void invisuable();//合法的，因为该函数可能是某个外部作用域的函数的引用，对于普通函数声明为友元函数不需要前置声明的
	//friend void ::error(){}//错误。同上，报错：error C2039: “error”: 不是“`global namespace'”的成员

	//为了记忆可以记住：非受限，不引用、可定义。（不是受限函数，就不能引用为友元函数，顺道可以做一下普通外部友元函数的定义）
};

template <typename T>
class TreeNode2{
	TreeNode2<T> * allocator();//模板成员函数
};

//模板类中的友元
template <typename T>
class List_TreeNode{
	friend TreeNode2<T> * TreeNode2::allocator();//引用友元模板成员函数
};

//当模板类中有了友元函数的定义的时候，会发生：

template <typename T>
class Creator{
	friend void appear(){
		//由前面例子讲的，这里可以定义appear（appear前面没有域作用符::）
	}
};
//但是在定义两个Creator模板对象的时候，有趣的事情就发生了：
Creator<double> miracle;//可以，这时候对模板进行实例化，生成了一个appear()
//Creator<int> oops;//错误，这时候对模板进行实例化，又生成了一个appear()，更有问题的是这两个定义是一样的，报错：
/*
	error C2084: 函数“void Creator<T>::appear(void)”已有主体
	with
	[
		T=double
	]
*/

//所以解决办法就是友元函数中必须带有类模板的模板参数，那么类似的正确的做法就是：

template <typename T>
class Creator2{
	friend void appear(T){//这样就会生成不同的函数模板实例
	}
};

Creator2<double> rightCreator;//可行
Creator2<int> alsoRightCreator;//可行

//如果将友元类、友元函数声明为模板，也就是友元模板，那么与该基本模板相应的所有的局部特化和显示特化都将会是所在类的友元

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
//这么一来，Task、Schedule::dispatch()、ticket()的模板实例都将是Manager的友元

#endif