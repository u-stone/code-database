#ifndef TEMPLATE_CLASS_H
#define TEMPLATE_CLASS_H

template<typename T>
class Danger{
public:
	enum {max = 10};
};
//����ʹ����Danger<void>::max����ô��ʵ������Danger<void>����ô�ػ�DangerΪvoid���͵�ʱ�򣬾��п�����Ϊ�����ļ�˳���ԭ����ɴ���

void clear(char * buf){
	for (int i = 0; i < Danger<void>::max; ++i){
		buf[i] = '\0';
	}
	cout << "Danger<void>::max = " << Danger<void>::max << endl;
}

char buffer[Danger<void>::max];//ʹ���˷���ֵ

void testBuffer(){
	clear(buffer);
}

#endif