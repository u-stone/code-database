#ifndef TEMPLATE_CORRECT_CLASS_H
#define TEMPLATE_CORRECT_CLASS_H

//下面是一个类模板特化定义正确的范例
template <typename T>
class Correct{
public:
	enum {value = 14};
};

template <>
class Correct<void>;

#endif