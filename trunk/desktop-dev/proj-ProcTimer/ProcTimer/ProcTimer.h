
// ProcTimer.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CProcTimerApp:
// �йش����ʵ�֣������ ProcTimer.cpp
//

class CProcTimerApp : public CWinAppEx
{
public:
	CProcTimerApp();

// ��д
	public:
	virtual BOOL InitInstance();

private:
	void CreateRun();
	bool m_bAutoStart;
// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CProcTimerApp theApp;