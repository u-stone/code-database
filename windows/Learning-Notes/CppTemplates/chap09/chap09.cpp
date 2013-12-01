// chap09.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "example.h"
#include "example2.h"

int count;


int _tmain(int argc, _TCHAR* argv[])
{
	testT1(NULL);
	BigNumber a, b;
	g(a, b);
	C<int> ca;
	testT2(&ca);
	testT3();
	testT4();
	Victim<void> bomb;
	boom(bomb);
	return 0;	
}

