// ComboEdit.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FileSearch.h"
#include "ComboEdit.h"
#include ".\comboedit.h"


// CComboEdit

IMPLEMENT_DYNAMIC(CComboEdit, CEdit)
CComboEdit::CComboEdit()
{
}

CComboEdit::~CComboEdit()
{
}


BEGIN_MESSAGE_MAP(CComboEdit, CEdit)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()



// CComboEdit ��Ϣ�������


void CComboEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CString str;
	GetWindowText(str);
	OutputDebugString(str + "\tOnChar\n");
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

void CComboEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	OutputDebugString(_T("CComboEdit::OnKeyDown\n"));
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}
