#ifndef EXAMPLE_CHAP15_H
#define EXAMPLE_CHAP15_H

#include <iostream>
using namespace std;


//trait��policy

//����ͨ��һ�����ӽ���ʹ��trait��policy����ģ�����е�һЩ����
//������Ӻܼ򵥣��������һ���ۼӵ�ģ��

//�ȿ���һ���汾
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
	//�����
	/*
	the average value of the integer values is 3
	the average value of the character in "templates" is -5
	*/
}
//������������е�����Ϊchar���͵�ʱ������˼������������Ϊchar�����ۼӵ�255֮��ͳ��������

//������������������������
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
	//�����
	/*
	the average value of the integer values is 3
	the average value of the character in "templates" is 108
	*/
}
//����Ĵ��������������һ��traitģ��

//ģ��Ա����õ����Ͷ���һ����������������������о�����ô����������
//���ȣ�ÿ�����Ͷ���Ĭ�Ϲ��캯�������ҹ����������ֵ��0
//�ڶ���ÿ�����Ͷ�֧��+=������������֧�ֽ�����
//����Ϊ�˼��ݸ������ͣ��������һ�����⣬�����ܱ�֤ÿ�����Ͷ���Ĭ�Ϲ��캯�������ҹ������ʼֵΪ0�������Ի���Ҫ�Ľ�
//Ϊ���������value trait
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
	//�����
	/*
	the average value of the integer values is 3
	the average value of the character in "templates" is 108
	*/
}

//�������������һ�����⣬����ֻ�����ͺ�ö�����Ͳ���������ڲ����о�̬�����ĳ�ʼ��
//��ô����ֻ�÷���Դ�ļ��н��г�ʼ����
//���ǣ������Ļ����������޷�֪��λ�������ļ��Ķ��壬��������ֻ��ʹ�������value trait

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
//ʹ�þ�̬�������س�ʼֵ����������û�������
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
	//�����
	/*
	the average value of the integer values is 3
	the average value of the character in "templates" is 108
	*/
}

//������������ʹ������Ĭ��ģ������ĺ���ģ��
//  template <typename T, typename AT = AccumulationTrait_v3<T>>
//  inline typename AT::AccT accum_v5(T const* beg, T const* end){
// 	 typedef at::AccT total = AT::zero();
// 	 while (beg != end){
// 		 total += *beg;
// 		 beg++;
// 	 }
// 	 return total;
//  }
//���Ϻ������屨��error C4519: ����������ģ����ʹ��Ĭ��ģ�����

//��������ֻ��ʹ����ģ���Ĭ��ģ�������
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
//Ϊ�˼�����ľ�̬������ʹ�ã����ǿ������������������

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
	//�����
	/*
	called first accum_help
	the average value of the integer values is 3
	called first accum_help
	the average value of the character in "templates" is 108
	*/
}
//ע�������testT5����û��ʹ�õ�Trait������ֻ�ǵ����˵�һ��help������Ҫʹ�õڶ���help����������Ҫ��ʾ����trait����

//����������ѧϰ��policy
//policy��trait�������һЩ��ͬ�㣬����traitע�������ͣ�policyע������Ϊ
//����������У����Ǽٶ��ۼӾ�����ͣ���������뻻һ���ۼӵĲ��ԣ���ô���ǿ�����������ķ���

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
	//�����
	/*
	called first accum_help
	the average value of the integer values is 3
	called first accum_help
	the average value of the character in "templates" is 108
	*/
}

//���ǻ������Զ���һ���µ�policy�㷨
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
	//�����
	/*
	the average value of the integer values is 0
	the average value of the character in "templates" is 0
	*/
}
//��testT7������Ľ���Ǵ���ģ�������Ϊtotal�ĳ�ʼֵ������Ϊ��0�����������˵����һ�����ʵĳ�ʼֵ�����Ƕ��ڳ˷���˵���Ǵ����


#endif