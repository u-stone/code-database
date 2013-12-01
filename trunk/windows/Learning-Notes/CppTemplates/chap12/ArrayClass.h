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
		//mark：这个函数只是交换给定的b和this数据的指针，没有涉及到大块的内存读写，所以速度很快
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
	//mark：这个函数的操作时进行了实际对象的拷贝，所以遇到大量数据的时候，会产生大量的数据拷贝过程，这样会拖慢程序运行的速度
}

#endif