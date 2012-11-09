// SearchDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FileSearch.h"
#include "SearchDlg.h"
#include ".\searchdlg.h"


// CSearchDlg �Ի���

IMPLEMENT_DYNAMIC(CSearchDlg, CDialog)
CSearchDlg::CSearchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSearchDlg::IDD, pParent)
{
	
}

CSearchDlg::~CSearchDlg()
{
}

void CSearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ComboSearch, m_ComboCtrl);
}


BEGIN_MESSAGE_MAP(CSearchDlg, CDialog)
	ON_WM_MEASUREITEM()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CSearchDlg ��Ϣ�������

BOOL CSearchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	//���������߳�
	MoveWindow(CRect(0, 0, 0, 0), FALSE);
	//BOOL ret = dsSkinWindow(GetDlgItem(IDC_ComboSearch)->GetSafeHwnd(), SKIN_TYPE_COMBOBOX, _T("combobox"), FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CSearchDlg::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	CClientDC dc(this);
	CFont *pFont = dc.SelectObject(GetFont());
	if (pFont != 0) 
	{
		TEXTMETRIC metrics;
		dc.GetTextMetrics(&metrics);

		lpMeasureItemStruct->itemHeight = metrics.tmHeight + metrics.tmExternalLeading;

		// An extra height of 2 looks good I think. 
		// Otherwise the list looks a bit crowded...
		if (lpMeasureItemStruct->itemID == -1)
			lpMeasureItemStruct->itemHeight += 2;
		else
			lpMeasureItemStruct->itemHeight += 
#ifdef PREVIEW_SHOW
			20;
#else
			10;
#endif

		dc.SelectObject(pFont);
	}

	CDialog::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}


BOOL CSearchDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_SPACE || pMsg->wParam == VK_RETURN)
			return 0;
		break;
	default:
		break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CSearchDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	m_ComboCtrl.MoveWindow(0, 0, cx, cy);
}
