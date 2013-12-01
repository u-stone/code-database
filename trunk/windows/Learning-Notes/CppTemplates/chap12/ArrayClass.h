#ifndef ARRAY_CLASS_H_CHAP12
#define ARRAY_CLASS_H_CHAP12

template <typename T>
class Array{
private:
	T* data;

public:
	Array(T* arg){
		data = arg;
	}
	Array(Array<T> const& arg){
		cout << "in Array<T> copy constructor " << endl;
		data = arg.data;
	}
	Array<T>& operator = (Array<T> const& param){
		if (this != &param)
			data = param.data;
		cout << "in Array<T> operator = " << endl;
		return *this;
	}

	void exchange_with(Array<T>* b){
		T* tmp = data;
		data = b->data;
		b->data = tmp;
		//mark���������ֻ�ǽ���������b��this���ݵ�ָ�룬û���漰�������ڴ��д�������ٶȺܿ�
	}
	T& operator[] (size_t k){
		cout << "in Array<T> operator [] " << endl;
		return data[k];
	}
};

template <typename T>
inline void exchange(T* a, T* b){
	T tmp(a);
	*a = *b;
	*b = tmp;
	//mark����������Ĳ���ʱ������ʵ�ʶ���Ŀ��������������������ݵ�ʱ�򣬻�������������ݿ������̣������������������е��ٶ�
}

#endif