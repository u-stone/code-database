// TestFileSearch.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "resource.h"		// ������


// CTestFileSearchApp:
// �йش����ʵ�֣������ TestFileSearch.cpp
//

class CTestFileSearchApp : public CWinApp
{
public:
	CTestFileSearchApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTestFileSearchApp theApp;
