#ifndef EXAMPLE_H_CHAP10
#define EXAMPLE_H_CHAP10

//��VC��gcc���ò���export����������ļ�ֻ��Ϊ�˸ñʼ������Զ�����
/*export */template <typename T>
	T const& max__(T const& a, T const&b){
		return a < b ? b : a;//(2)
}

#endif