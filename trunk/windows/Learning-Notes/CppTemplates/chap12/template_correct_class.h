#ifndef TEMPLATE_CORRECT_CLASS_H
#define TEMPLATE_CORRECT_CLASS_H

//������һ����ģ���ػ�������ȷ�ķ���
template <typename T>
class Correct{
public:
	enum {value = 14};
};

template <>
class Correct<void>;

#endif