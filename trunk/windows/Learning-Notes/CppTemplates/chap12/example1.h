#ifndef EXAMPLE_CHAP12_H
#define EXAMPLE_CHAP12_H

template <typename T1,typename T2>
void f3(T2, T1){}

void g(){
	f3<char, char>('a', 'b');
}

#endif