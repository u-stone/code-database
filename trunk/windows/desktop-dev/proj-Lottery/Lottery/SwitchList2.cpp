#include "StdAfx.h"
#include "SwitchList2.h"

SwitchList2::SwitchList2(void)
{
}

SwitchList2::~SwitchList2(void)
{
}

//将数据写入list中的下一个数据单元
void SwitchList2::putData(void* pDataUnit)
{
	if (m_OpType == 0)//表示当前状态是List1为R，List2为W
	{
		
	}
	else if (m_OpType == 1)//表示当前状态是List1为W，List2为R
	{
		
	}
}
//使用可读List中的下一个数据单元，并结合pParam来进行数据展示
void SwitchList2::useData(void* pParam)
{
	
}