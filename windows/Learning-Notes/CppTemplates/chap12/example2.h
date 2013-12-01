#ifndef _EXAMPLE2_CHAP12_H
#define _EXAMPLE2_CHAP12_H

template<typename T>
class Danger;

template<>
class Danger<void>{
public:
	enum{max = 100};
};

void clear(char const* buf){
	for (int k = 0; k < Danger<void>::max; ++i)
		buf[k] = '\0';
}


#endif