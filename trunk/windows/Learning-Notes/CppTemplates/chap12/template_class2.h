#ifndef TEMPLATE_CLASS2_H
#define TEMPLATE_CLASS2_H

//�����������������Danger���ػ�����ʶtemplate<>���
template <typename T>
class Danger;

template <>
class Danger<void>{
public:
	enum {max = 100};
};

#endif