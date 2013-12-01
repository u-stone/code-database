#ifndef TEMPLATE_CLASS2_H
#define TEMPLATE_CLASS2_H

//这个必须声明，否则Danger的特化不认识template<>标记
template <typename T>
class Danger;

template <>
class Danger<void>{
public:
	enum {max = 100};
};

#endif