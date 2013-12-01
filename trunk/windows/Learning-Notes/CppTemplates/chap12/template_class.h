#ifndef TEMPLATE_CLASS_H
#define TEMPLATE_CLASS_H

template<typename T>
class Danger{
public:
	enum {max = 10};
};
//下面使用了Danger<void>::max，那么就实例化了Danger<void>，那么特化Danger为void类型的时候，就有可能因为引用文件顺序的原因，造成错误

void clear(char * buf){
	for (int i = 0; i < Danger<void>::max; ++i){
		buf[i] = '\0';
	}
	cout << "Danger<void>::max = " << Danger<void>::max << endl;
}

char buffer[Danger<void>::max];//使用了泛型值

void testBuffer(){
	clear(buffer);
}

#endif