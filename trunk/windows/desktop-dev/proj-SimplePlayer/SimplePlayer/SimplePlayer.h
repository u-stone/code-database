
// SimplePlayer.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSimplePlayerApp:
// �йش����ʵ�֣������ SimplePlayer.cpp
//

class CSimplePlayerApp : public CWinApp
{
public:
	CSimplePlayerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CSimplePlayerApp theApp;