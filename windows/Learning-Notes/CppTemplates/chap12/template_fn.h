#ifndef TEMPLATE_FN_HCAP12_H
#define TEMPLATE_FN_HCAP12_H


//模板定义
template <typename T>
int fn(T, T x = 42){
	return x;
}
//特化声明紧跟其后，这样可以禁止模板实例化
template <>
int fn(int, int y);

#endif