#ifndef TEMPLATE_FN_HCAP12_H
#define TEMPLATE_FN_HCAP12_H


//ģ�嶨��
template <typename T>
int fn(T, T x = 42){
	return x;
}
//�ػ�������������������Խ�ֹģ��ʵ����
template <>
int fn(int, int y);

#endif