#ifndef EXAMPLE_CHAP15_H
#define EXAMPLE_CHAP15_H

#include <iostream>
using namespace std;


//trait与policy

//本章通过一个例子讲解使用trait、policy来简化模板编程中的一些参数
//这个例子很简单，就是完成一个累加的模板

//先看第一个版本
template <typename T>
inline T accum_v1(T const *beg, T const* end){
	T total = T();
	while (beg != end){
		total += *beg;
		beg++;
	}
	return total;
}

void testT1(){
	int num[] = {1,2,3,4,5};
	cout << "the average value of the integer values is "
		<< accum_v1(&num[0], &num[5])/5
		<< endl;

	char name[] = "templates";
	int length = sizeof(name) - 1;
	cout << "the average value of the character in \""
		<< name << "\" is "
		<< accum_v1(&name[0], &name[length]) / length
		<< endl;
	//输出：
	/*
	the average value of the integer values is 3
	the average value of the character in "templates" is -5
	*/
}
//在上面的例子中当参数为char类型的时候出现了计算错误，这是因为char类型累加到255之后就出现了溢出

//下面针对这个问题提出解决方案
template <typename T>
class AccumulationTraits_v1;

template <>
class AccumulationTraits_v1<char>{
public:
	typedef int AccT;
};

template <>
class AccumulationTraits_v1<short>{
public:
	typedef int AccT;
};

template <>
class AccumulationTraits_v1<int>{
public:
	typedef long AccT;
};

template <>
class AccumulationTraits_v1<unsigned int>{
public:
	typedef unsigned long AccT;
};

template <>
class AccumulationTraits_v1<float>{
public:
	typedef double AccT;
};

template <typename T>
inline typename AccumulationTraits_v1<T>::AccT accum_v2(T const* beg, T const* end){
	typedef typename AccumulationTraits_v1<T>::AccT AccT;

	AccT total = AccT();
	while (beg != end){
		total += *beg;
		beg++;
	}
	return total;
}

void testT2(){
	int num[] = {1,2,3,4,5};
	cout << "the average value of the integer values is "
		<< accum_v2(&num[0], &num[5])/5
		<< endl;

	char name[] = "templates";
	int length = sizeof(name) - 1;
	cout << "the average value of the character in \""
		<< name << "\" is "
		<< accum_v2(&name[0], &name[length]) / length
		<< endl;
	//输出：
	/*
	the average value of the integer values is 3
	the average value of the character in "templates" is 108
	*/
}
//上面的代码中我们添加了一个trait模板

//模板对被调用的类型都有一个期望，我们上面的例子中就有这么两种期望：
//首先，每种类型都有默认构造函数，并且构造出来的数值是0
//第二，每种类型都支持+=操作符，并且支持解引用
//我们为了兼容更多类型，这里就有一个问题，并不能保证每种类型都有默认构造函数，并且构造出初始值为0来，所以还需要改进
//为此我们添加value trait
template <typename T>
class AccumulationTrait_v2;

template <>
class AccumulationTrait_v2<char>{
public:
	typedef int AccT;
	static AccT const zero = 0;
};

template <>
class AccumulationTrait_v2<short>{
public:
	typedef int AccT;
	static AccT const zero = 0;
};

template <>
class AccumulationTrait_v2<int>{
public:
	typedef long AccT;
	static AccT const zero = 0;
};

template <>
class AccumulationTrait_v2<unsigned int>{
public:
	typedef unsigned long AccT;
	static AccT const zero = 0;
};

//  template <>
//  class AccumulationTrait_v2<float>{
//  public:
// 	 typedef double AccT;
// 	 static AccT const zero = 0.0;
//  };


template <typename T>
inline typename AccumulationTrait_v2<T>::AccT accum_v3(T const* beg, T const* end){
	typedef typename AccumulationTrait_v2<T>::AccT AccT;

	AccT total = AccumulationTrait_v2<T>::zero;
	while (beg != end){
		total += *beg;
		beg++;
	}
	return total;
}

void testT3(){
	int num[] = {1,2,3,4,5};
	cout << "the average value of the integer values is "
		<< accum_v3(&num[0], &num[5])/5
		<< endl;

	char name[] = "templates";
	int length = sizeof(name) - 1;
	cout << "the average value of the character in \""
		<< name << "\" is "
		<< accum_v3(&name[0], &name[length]) / length
		<< endl;
	//输出：
	/*
	the average value of the integer values is 3
	the average value of the character in "templates" is 108
	*/
}

//但是这个方案有一个问题，就是只有整型和枚举类型才能在类的内部进行静态变量的初始化
//那么我们只好放在源文件中进行初始化。
//但是，这样的话，编译器无法知道位于其他文件的定义，所以我们只能使用下面的value trait

template <typename T>
class AccumulationTrait_v3;

template <>
class AccumulationTrait_v3<char>{
public:
	typedef int AccT;
	static AccT zero(){
		return 0;
	}
};

template <>
class AccumulationTrait_v3<short>{
public:
	typedef int AccT;
	static AccT zero(){
		return 0;
	}
};

template <>
class AccumulationTrait_v3<int>{
public:
	typedef long AccT;
	static AccT zero(){
		return 0;
	}
};

template <>
class AccumulationTrait_v3<unsigned int>{
public:
	typedef unsigned long AccT;
	static AccT zero(){
		return 0;
	}
};

template <>
class AccumulationTrait_v2<float>{
public:
	typedef double AccT;
	static AccT const zero(){
		return 0.0;
	}
};

template <typename T>
inline typename AccumulationTrait_v3<T>::AccT accum_v4(T const* beg, T const* end){
	typedef typename AccumulationTrait_v3<T>::AccT AccT;

	AccT total = AccumulationTrait_v3<T>::zero();
	while (beg != end){
		total += *beg;
		beg++;
	}
	return total;
}
//使用静态函数返回初始值，这样总是没有问题滴
void testT4(){
	int num[] = {1,2,3,4,5};
	cout << "the average value of the integer values is "
		<< accum_v4(&num[0], &num[5])/5
		<< endl;

	char name[] = "templates";
	int length = sizeof(name) - 1;
	cout << "the average value of the character in \""
		<< name << "\" is "
		<< accum_v4(&name[0], &name[length]) / length
		<< endl;
	//输出：
	/*
	the average value of the integer values is 3
	the average value of the character in "templates" is 108
	*/
}

//下面我们试试使用设置默认模板参数的函数模板
//  template <typename T, typename AT = AccumulationTrait_v3<T>>
//  inline typename AT::AccT accum_v5(T const* beg, T const* end){
// 	 typedef at::AccT total = AT::zero();
// 	 while (beg != end){
// 		 total += *beg;
// 		 beg++;
// 	 }
// 	 return total;
//  }
//以上函数定义报错：error C4519: 仅允许在类模板上使用默认模板参数

//所以我们只能使用类模板的默认模板参数了
template <typename T, typename AT = AccumulationTrait_v3<T>>
class Accum{
public:
	static typename AT::AccT accum(T const* beg, T const* end){
		typename AT::AccT total = AT::zero();
		while (beg != end){
			total += *beg;
			++beg;
		}
		return total;
	}
};
//为了简化上面的静态函数的使用，我们可以添加两个辅助函数

template <typename T>
inline typename AccumulationTrait_v3<T>::AccT accum_help(T const* beg, T const* end){
	cout << "called first accum_help" << endl;
	return Accum<T>::accum(beg, end);
}

template <typename Traits, typename T>
inline typename AccumulationTrait_v3<T>::AccT accum_help(T const* beg, T const* end){
	cout << "called second accum_help" << endl;
	return Accum<T, Traits>::accum(beg, end);
}

void testT5(){
	int num[] = {1,2,3,4,5};
	cout << "the average value of the integer values is "
		<< accum_help(&num[0], &num[5])/5
		<< endl;

	char name[] = "templates";
	int length = sizeof(name) - 1;
	cout << "the average value of the character in \""
		<< name << "\" is "
		<< accum_help(&name[0], &name[length]) / length
		<< endl;
	//输出：
	/*
	called first accum_help
	the average value of the integer values is 3
	called first accum_help
	the average value of the character in "templates" is 108
	*/
}
//注意上面的testT5函数没有使用到Trait参数，只是调用了第一个help函数，要使用第二个help函数我们需要显示设置trait参数

//接下来我们学习下policy
//policy和trait这二者有一些共同点，但是trait注重于类型，policy注重于行为
//上面的例子中，我们假定累加就是求和，如果我们想换一下累加的策略，那么我们可以试试下面的方法

class SumPolicy{
public:
	template <typename T1, typename T2>
	static void accumulate(T1& total, T2 const& value){
		total += value;
	}
};

template <typename T,
	typename Policy = SumPolicy,
	typename Traits = AccumulationTrait_v3<T>>
class Accum_v2{
public:
	typedef typename Traits::AccT AccT;
	static AccT accum(T const* beg, T const* end){
		AccT total = Traits::zero();
		while (beg != end){
			Policy::accumulate(total, *beg);
			++beg;
		}
		return total;
	}
};

void testT6(){
	int num[] = {1,2,3,4,5};
	cout << "the average value of the integer values is "
		<< Accum_v2<int>::accum(&num[0], &num[5])/5
		<< endl;

	char name[] = "templates";
	int length = sizeof(name) - 1;
	cout << "the average value of the character in \""
		<< name << "\" is "
		<< Accum_v2<char>::accum(&name[0], &name[length]) / length
		<< endl;
	//输出：
	/*
	called first accum_help
	the average value of the integer values is 3
	called first accum_help
	the average value of the character in "templates" is 108
	*/
}

//我们还可以自定义一个新的policy算法
class MultiPolicy{
public:
	template <typename T1, typename T2>
	static void accumulate(T1& total, T2 const& value){
		total *= value;
	}
};

void testT7(){
	int num[] = {1,2,3,4,5};
	cout << "the average value of the integer values is "
		<< Accum_v2<int, MultiPolicy>::accum(&num[0], &num[5])/5
		<< endl;

	char name[] = "templates";
	int length = sizeof(name) - 1;
	cout << "the average value of the character in \""
		<< name << "\" is "
		<< Accum_v2<char, MultiPolicy>::accum(&name[0], &name[length]) / length
		<< endl;
	//输出：
	/*
	the average value of the integer values is 0
	the average value of the character in "templates" is 0
	*/
}
//在testT7中输出的结果是错误的，这是因为total的初始值被设置为了0，对于求和来说这是一个合适的初始值，但是对于乘法来说这是错误的


#endif