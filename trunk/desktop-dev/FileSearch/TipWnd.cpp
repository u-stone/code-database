// TipWnd.cpp : implementation file
//

#include "stdafx.h"
#include "TipWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define FONT_HEIGHT -18

CTipWnd::CTipWnd()
{
}

CTipWnd::~CTipWnd()
{
}

BOOL CTipWnd::Create(CWnd* pParent)
{
	m_font.CreateFont(-13, 0, 0, 0, FW_NORMAL, 0, 0, 0, GB2312_CHARSET, 
		OUT_CHARACTER_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_SWISS | FF_MODERN, _T("ËÎÌו"));
	return 	CWnd::CreateEx(WS_EX_TOOLWINDOW,
		AfxRegisterWndClass(0),
		NULL,
		WS_POPUPWINDOW | WS_CHILDWINDOW,
		0,
		0,0,0,
		pParent->GetSafeHwnd(),
		(HMENU)0);
}


BEGIN_MESSAGE_MAP(CTipWnd, CWnd)
	//{{AFX_MSG_MAP(CTipWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CTipWnd::OnEraseBkgnd(CDC* pDC) 
{
	CBrush br(GetSysColor(COLOR_INFOBK));
	CRect rc;
	pDC->GetClipBox(rc);
	CBrush* pOldBrush = pDC->SelectObject(&br);
	pDC->PatBlt(rc.left,rc.top,rc.Width(),rc.Height(),PATCOPY);
	pDC->SelectObject(pOldBrush);
	return TRUE;
}

void CTipWnd::ShowTips(CPoint pt,const CString& str)
{
	CSize sz;
	CDC* pDC = GetDC();
	if (m_strFont != str)
	{
		m_strFont = str;
		CFont* pFont = pDC->SelectObject(&m_font);
		sz = pDC->GetTextExtent(m_strFont);
		sz.cx += 8;
		sz.cy += 8;
		pDC->SelectObject(pFont);
		ReleaseDC(pDC);
		SetWindowPos(0,pt.x,pt.y,sz.cx,sz.cy,SWP_SHOWWINDOW|SWP_NOACTIVATE);
		RedrawWindow();
	}
}

void CTipWnd::OnPaint() 
{
	CPaintDC dc(this);
	
	CFont* pf = dc.SelectObject(&m_font);
	CRect rc;
	GetClientRect(rc);
	dc.DrawText(m_strFont,rc,DT_SINGLELINE|DT_VCENTER|DT_CENTER);
	dc.SelectObject(pf);
}
