// FileSearch.h : FileSearch DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ������


// CFileSearchApp
// �йش���ʵ�ֵ���Ϣ������� FileSearch.cpp
//

class CFileSearchApp : public CWinApp
{
public:
	CFileSearchApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
