#ifndef TEMPLATE_FN2_H
#define  TEMPLATE_FN2_H

template <typename T>
int fn2(T x, T y = 28)
{
	return x;
}

template <>
inline int fn2(int x, int y /* = 28 */){
	return y/2;
}

#endif