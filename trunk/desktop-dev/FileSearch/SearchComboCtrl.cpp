#include "StdAfx.h"
#include ".\searchcomboctrl.h"

static WNDPROC m_pWndProc = NULL;
static CSearchComboCtrl *m_pComboBox = NULL;

BEGIN_MESSAGE_MAP(CSearchComboCtrl, CComboBox)
	ON_WM_CREATE()
	ON_MESSAGE(WM_RESETLISTBOX, ResetListBox)
	ON_CONTROL_REFLECT(CBN_EDITCHANGE, OnCbnEditchange)
	ON_CONTROL_REFLECT_EX(CBN_DROPDOWN, OnDropDown)
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseUp)
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(WM_CTLCOLORLISTBOX, OnCtlColorListBox)
	ON_MESSAGE(CB_INSERTSTRING, OnAddString)
	ON_MESSAGE(CB_ADDSTRING, OnAddString)
	ON_MESSAGE(CB_DELETESTRING, OnDeleteString)
	ON_MESSAGE(CB_RESETCONTENT, OnResetContent)
	ON_MESSAGE(WM_EDITCHANGE, OnEditChange)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_TIMER()
	ON_CONTROL_REFLECT(CBN_SETFOCUS, OnCbnSetfocus)
	ON_WM_DESTROY()
	ON_WM_PAINT()
END_MESSAGE_MAP()

extern "C" LRESULT FAR PASCAL ComboBoxListBoxProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	static bool bCloseOnLButtonUp = false;
	switch (nMsg)
	{
	case WM_CHAR:
		{
			if (wParam == VK_SPACE && (lParam & 1 << 30) == 0)
			{
				// Get the current selection
				INT nIndex = (INT)CallWindowProc(m_pWndProc, hWnd, LB_GETCURSEL, wParam, lParam);

				CRect rcItem;
				SendMessage(hWnd, LB_GETITEMRECT, nIndex, (LPARAM)(VOID *)&rcItem);
				InvalidateRect(hWnd, rcItem, FALSE);
				// Invert the check mark
				m_pComboBox->SetCheck(nIndex, !m_pComboBox->GetCheck(nIndex));

				// Notify that check state has changed
				m_pComboBox->GetParent()->SendMessage(WM_COMMAND, MAKELONG(GetWindowLong(m_pComboBox->m_hWnd, GWL_ID), BN_CLICKED), (LPARAM)m_pComboBox->m_hWnd);
				return 0;
			}
			break;
		}
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
		{
			CRect rc;
			CRect rrc;
			GetClientRect(hWnd, rc);
			CPoint pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);
			if (PtInRect(rc, pt))
			{
				INT nItemHeight = (INT)SendMessage(hWnd, LB_GETITEMHEIGHT, 0, 0);
				INT nTopIndex   = (INT)SendMessage(hWnd, LB_GETTOPINDEX, 0, 0);
				// Compute which index to check/uncheck
				INT nIndex = nTopIndex + pt.y / nItemHeight;
				SendMessage(hWnd, LB_GETITEMRECT, nIndex, (LPARAM)(VOID *)&rc);
				if (PtInRect(rc, pt))
				{
					rrc.CopyRect(rc);
					rrc.left = rrc.right - (long)(nItemHeight * PREVIEW_RATIO);
					// Invalidate this window
					InvalidateRect(hWnd, rc, FALSE);
					InvalidateRect(hWnd, rrc, TRUE);
					// Notify that selection has changed
					m_pComboBox->GetParent()->SendMessage(WM_COMMAND, MAKELONG(GetWindowLong(m_pComboBox->m_hWnd, GWL_ID), CBN_SELCHANGE), (LPARAM)m_pComboBox->m_hWnd);
					// Was the click on the check box?
					HDC hdc = GetDC(hWnd);
					TEXTMETRIC metrics;
					GetTextMetrics(hdc, &metrics);
					ReleaseDC(hWnd, hdc);
					rc.right = rc.left + metrics.tmHeight + metrics.tmExternalLeading + 6;
					if (PtInRect(rc, pt))//�ж��Ƿ����˸�ѡ��ť
					{
#ifdef CHECK_SHOW
						// Toggle the check state
						m_pComboBox->SetCheck(nIndex, !m_pComboBox->GetCheck(nIndex));
						// Notify that check state has changed
						m_pComboBox->GetParent()->SendMessage(WM_COMMAND, MAKELONG(GetWindowLong(m_pComboBox->m_hWnd, GWL_ID), BN_CLICKED), (LPARAM)m_pComboBox->m_hWnd);
#else
						bCloseOnLButtonUp = true;
#endif
					}
					else if (PtInRect(rrc, pt))//�ж��Ƿ�����Ԥ����ť
					{
						// Toggle the check state
						m_pComboBox->SetPreview(nIndex, !m_pComboBox->GetPreview(nIndex));
						// Notify that check state has changed
						m_pComboBox->GetParent()->SendMessage(WM_COMMAND, MAKELONG(GetWindowLong(m_pComboBox->m_hWnd, GWL_ID), BN_CLICKED), (LPARAM)m_pComboBox->m_hWnd);
					}
					else
						bCloseOnLButtonUp = true;
				}
			}
			// Do the default handling now
			break;
		}
	case WM_LBUTTONUP:
		{
			if (!bCloseOnLButtonUp)
				return 0;
			bCloseOnLButtonUp = false;
		}
		break;
	case WM_KEYDOWN:
		{
			switch (wParam)
			{
			case VK_RETURN:
			case VK_ESCAPE:
			case VK_SPACE:
				return 0;
			}
		}
		break;
	default:
		break;
	}
	return CallWindowProc(m_pWndProc, hWnd, nMsg, wParam, lParam);
}

CSearchComboCtrl::CSearchComboCtrl(void)
{
	m_bDelete = false;
	m_bHistory = true;
	m_hListBox = NULL;
	m_wndTip.Create(this);
}

CSearchComboCtrl::~CSearchComboCtrl(void)
{
}

int CSearchComboCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

void CSearchComboCtrl::ToSearch()//׼�����������ַ���
{
	if (!m_DWGFileSearcher.start())
	{
		OutputDebugString(_T("û�н���������������û�д�����������!"));
	}
}
bool CSearchComboCtrl::BuildIndex(){
	return m_DWGFileSearcher.BuildIndex();
}
bool CSearchComboCtrl::AddSearchFolder(string pAddedFolder)
{
	return m_DWGFileSearcher.AddUserFolder(pAddedFolder);
}
bool CSearchComboCtrl::GetResult(list<pair<string, string> >& sFilesSet)
{
	return m_DWGFileSearcher.GetSearchResult(sFilesSet);
}
string CSearchComboCtrl::GetFilePathSelected()
{
	string strPath;
	int index = 0;
	int iCurSel = GetCurSel();
	if (m_bHistory)
	{
		for (set<pair<string, string> >::iterator iter = m_UserSearchHistory.begin(); iter != m_UserSearchHistory.end(); ++iter)
		{
			if (index ++ == iCurSel)
			{
				strPath = iter->second.c_str();
				break;
			}
		}
	}
	else
	{
		for (list<pair<string, string> >::iterator iter = m_Results.begin(); iter != m_Results.end(); ++iter)
		{
			if (index ++ == iCurSel)
			{
				strPath = iter->second.c_str();
				break;
			}
		}				
	}
	return strPath;
}
LRESULT CSearchComboCtrl::ResetListBox(WPARAM wParam, LPARAM lParam)
{
	static bool bFinishSearch = true;
	m_Results.clear();
	m_PreviewArray.RemoveAll();
	static CString str;
	GetWindowText(str);
	ResetContent();
	SetWindowText(str);
	if (!str.IsEmpty())
	{
		m_bHistory = false;
		bFinishSearch = m_DWGFileSearcher.GetSearchResult(m_Results);//����µĽ��
		if (!bFinishSearch)
		{
			PostMessage(WM_RESETLISTBOX, str.GetLength(), str.GetLength());
			return 0;
		}
		int index = 0;
		CString tmp;
		size_t height = GetItemHeight(0) * m_Results.size();
		for (list<pair<string, string> >::iterator iter = m_Results.begin(); iter != m_Results.end(); ++iter)
			AddString(iter->first.c_str());
		if (GetCount() > 0)
		{
			PostMessage(CBN_SELCHANGE, 0, 0);//����ѡ����ı����Ϣ����ʾ��ǰĬ��ѡ�������ϸ��Ϣ
			SetTopIndex(0);//ѡ��ָ����
		}
		RecalcDropWidth();
		ShowDropDown(FALSE);
		if (!str.IsEmpty() && !m_Results.empty())
			ShowDropDown(TRUE);//��������������������ʹ�������б�����Ӧ��Ƶ��༭����
		SetCursor(LoadCursor(NULL,IDC_ARROW));
		static CString newstr;
		if (!m_bDelete)
		{
			GetWindowText(newstr);
			if (newstr.GetLength() > str.GetLength())
			{
				if (str.CompareNoCase(newstr.Left(str.GetLength())) == 0)
					str.Format(_T("%s%s"), newstr.Left((int)lParam), newstr.Right(newstr.GetLength() - (int)lParam));
			}
		}
		SetWindowText(str);
		SetEditSel((int)lParam, str.GetLength());
		//if (!bFinishSearch)//û��������ɣ���ô�ͽ�����һ�ε��������
		//{
		//	static DWORD sel = 0;
		//	sel = GetEditSel();
		//	OutputDebugString(str + " �ٴη�������\n");
		//	//�������������б�
		//	PostMessage(CBN_EDITCHANGE);
		//	//PostMessage(WM_RESETLISTBOX, (WPARAM)LOWORD(sel), (LPARAM)HIWORD(sel));
		//}
	}
	else
	{
		ShowDropDown(FALSE);
		RecalcDropWidth();
		m_bHistory = true;
		ReadUserSearchHisroty();
		for (set<pair<string, string> >::iterator iter = m_UserSearchHistory.begin(); iter != m_UserSearchHistory.end(); ++iter)
			AddString(iter->first.c_str());
	}
	return 0;
}

void CSearchComboCtrl::OnCbnEditchange()
{
	EditCtrlChanged();
}
void CSearchComboCtrl::EditCtrlChanged()
{
	CString str;
	GetWindowText(str); 
	m_bDelete = false;
	if (GetAsyncKeyState(VK_BACK) < 0)
		m_bDelete = true;
	DWORD sel = GetEditSel();
	static CString strc;
	GetWindowText(strc);
	m_DWGFileSearcher.SearchStrChange(strc.GetBuffer());
	strc.ReleaseBuffer();
	//�������������б�
	PostMessage(WM_RESETLISTBOX, (WPARAM)LOWORD(sel), (LPARAM)HIWORD(sel));	
}
void CSearchComboCtrl::OnPaint()
{
	//һ���������֣��༭�� + ������ť����Ҫ�ֱ����
	CPaintDC dc(this); // device context for painting
	CRect rect;
	GetClientRect(&rect);
	dc.SelectStockObject(NULL_BRUSH);
	dc.Rectangle(&rect);
	CRgn rgn;
	rgn.CreateRoundRectRgn(rect.left, rect.top, rect.right, rect.bottom, 1, 1);
	
	rect.left = rect.right - GetSystemMetrics(SM_CYVSCROLL);
	CBrush dbrush, *poldbrush;
	dbrush.CreateSolidBrush(RGB(255, 0, 0));
	poldbrush = dc.SelectObject(&dbrush);
	rect.DeflateRect(-1, 1, 1, 1);
	//dc.FillRgn(&rgn, &dbrush);
	dc.DrawFrameControl(&rect, DFC_SCROLL, DFCS_SCROLLDOWN);
	dc.SelectObject(poldbrush);
	dbrush.DeleteObject();
}
void CSearchComboCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (lpDrawItemStruct->CtlType != ODT_COMBOBOX)
		return ;
	CDC dc;

	dc.Attach(lpDrawItemStruct->hDC);
	dc.SetBkMode(TRANSPARENT);

	static CString strtxt;
	if (lpDrawItemStruct->itemID == -1)
	{
		OutputDebugString("no string\n");
		dc.Detach();
		return ;
	}
	GetLBText(lpDrawItemStruct->itemID, strtxt);
	// Save these value to restore them when done drawing.
	COLORREF crOldTextColor = dc.GetTextColor();
	COLORREF crOldBkColor = dc.GetBkColor();
	//////////////////////////////////////////////////////////////////////////
	//���Ƹ�ѡ��
	DrawCheckbtn(dc, lpDrawItemStruct);
	//����Ԥ��ͼ
	DrawPreview(&dc, lpDrawItemStruct);
	//���Ƹ�������
	DrawListItem(&dc, lpDrawItemStruct);
	//////////////////////////////////////////////////////////////////////////
	//�����ı�
	RECT rcPreview;
	memcpy(&rcPreview, &lpDrawItemStruct->rcItem, sizeof(RECT));
	rcPreview.left += 10;//�����ڸ�����ʾ֮��һ��ĵط���ʼ����
	dc.SetTextColor(RGB(50, 50, 50));
	dc.DrawText(
		strtxt,
		strtxt.GetLength(),
		&rcPreview,
		DT_LEFT|DT_SINGLELINE|DT_VCENTER);
	//////////////////////////////////////////////////////////////////////////
	dc.SetTextColor(crOldTextColor);
	dc.SetBkColor(crOldBkColor);
	dc.Detach();
}
void CSearchComboCtrl::DrawEdit(CDC* pDC, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	
}
void CSearchComboCtrl::DrawListItem(CDC* pDC, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if ((lpDrawItemStruct->itemAction | ODA_SELECT) &&
		(lpDrawItemStruct->itemState  & ODS_SELECTED))
	{
		//pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		//pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
		//pDC->FillSolidRect(&lpDrawItemStruct->rcItem, ::GetSysColor(COLOR_HIGHLIGHT));
		pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		pDC->SetBkColor(RGB(160, 160, 160));
		pDC->FillSolidRect(&lpDrawItemStruct->rcItem, RGB(160, 160, 160));
		pDC->SelectStockObject(BLACK_PEN);
		pDC->SelectStockObject(NULL_BRUSH);
		pDC->Rectangle(&lpDrawItemStruct->rcItem);
		SetTimer(1, 20, NULL);
	}
	else
		//pDC->FillSolidRect(&lpDrawItemStruct->rcItem, pDC->GetBkColor());
		pDC->FillSolidRect(&lpDrawItemStruct->rcItem, RGB(225, 225, 225));
}
void CSearchComboCtrl::DrawCheckbtn(HDC hdc, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
#ifdef CHECK_SHOW
	RECT rcCheck;
	memcpy(&rcCheck, &lpDrawItemStruct->rcItem, sizeof(RECT));
	rcCheck.right = rcCheck.left + 20;
	rcCheck.left += 1;
	rcCheck.top += 1;
	rcCheck.right -= 1;
	rcCheck.bottom -= 1;
	// If this item is selected, set the background color 
	// and the text color to appropriate values. Erase
	// the rect by filling it with the background color.
	//�Ƿ���ʾ��ѡ��
	lpDrawItemStruct->rcItem.left += 20;//������������
	INT nCheck = 0;
	if (static_cast<int>(lpDrawItemStruct->itemID) >= 0)
		nCheck = 1 + GetCheck(lpDrawItemStruct->itemID);
	if (nCheck > 0)
	{
		SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
		SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));

		UINT nState = DFCS_BUTTONCHECK;

		if (nCheck > 1)
			nState |= DFCS_CHECKED;

		if (!IsWindowEnabled())
			nState |= DFCS_INACTIVE;
		// Draw the checkmark using DrawFrameControl
		DrawFrameControl(lpDrawItemStruct->hDC, &rcCheck, DFC_BUTTON, nState);
	}
#endif
}
void CSearchComboCtrl::DrawPreviewImage(CDC* pDC, CString csFileName, RECT rect)
{
	HBITMAP hbmp = ReadDWGPreviewImageData(pDC, csFileName);
	rect.left += 3;
	rect.right -= 3;
	rect.top += 3;
	rect.bottom -= 3;
	if(hbmp != NULL)
	{
		CDC memDC;  memDC.CreateCompatibleDC(0);
		HBITMAP hOldBitmap = (HBITMAP)memDC.SelectObject(hbmp);
		BITMAP bitmapInfo; 
		CBitmap::FromHandle(hbmp)->GetBitmap(&bitmapInfo);
		::StretchBlt(pDC->m_hDC,rect.left,rect.top,rect.right - rect.left,rect.bottom - rect.top,memDC.m_hDC,0,0,bitmapInfo.bmWidth,bitmapInfo.bmHeight,SRCCOPY);
		memDC.SelectObject(hOldBitmap);
		memDC.DeleteDC();
	}
	else
	{
		CString strtxt = _T("��Ԥ��");
		pDC->DrawText(
			strtxt,
			strtxt.GetLength(),
			&rect,
			DT_LEFT|DT_SINGLELINE|DT_VCENTER);
	}
}
void CSearchComboCtrl::DrawPreview(CDC* pDC, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
#ifdef PREVIEW_SHOW
	lpDrawItemStruct->rcItem.right -= (long)(GetItemHeight(lpDrawItemStruct->itemID) * PREVIEW_RATIO);
	RECT rcPreview;
	int nPreview = 0;
	if (static_cast<int>(lpDrawItemStruct->itemID) >= 0)
	{
		if (!GetPreview(lpDrawItemStruct->itemID))
		{
			memcpy(&rcPreview, &lpDrawItemStruct->rcItem, sizeof(RECT));
			rcPreview.left = rcPreview.right;
			rcPreview.right += (long)(GetItemHeight(lpDrawItemStruct->itemID) * PREVIEW_RATIO);
			rcPreview.left += 5;
			rcPreview.top += 5;
			rcPreview.right -= 5;
			rcPreview.bottom -= 5;
			UINT nState = DFCS_BUTTONPUSH | DFCS_FLAT;
			if (!IsWindowEnabled())
				nState |= DFCS_INACTIVE;
			DrawFrameControl(lpDrawItemStruct->hDC, &rcPreview, DFC_BUTTON, nState);
			static CString strpre = _T("Ԥ��");
			DrawText(lpDrawItemStruct->hDC,
				strpre,
				strpre.GetLength(),
				&rcPreview,
				DT_CENTER|DT_SINGLELINE|DT_VCENTER);
		}
		else// if (GetPreview(lpDrawItemStruct->itemID))	//����Ԥ��ͼƬ
		{
			static CString strFilePath;
			int index = 0;
			if (m_bHistory)
			{
				for (set<pair<string, string> >::iterator iter = m_UserSearchHistory.begin(); iter != m_UserSearchHistory.end(); ++iter)
				{
					if (index ++ == lpDrawItemStruct->itemID)
					{
						strFilePath = iter->second.c_str();
						break;
					}
				}
			}
			else
			{
				for (list<pair<string, string> >::iterator iter = m_Results.begin(); iter != m_Results.end(); ++iter)
				{
					if (index ++ == lpDrawItemStruct->itemID)
					{
						strFilePath = iter->second.c_str();
						break;
					}
				}				
			}
			//������εĻ��Ʒ�Χ
			memcpy(&rcPreview, &lpDrawItemStruct->rcItem, sizeof(RECT));
			rcPreview.left = lpDrawItemStruct->rcItem.right;
			rcPreview.right = rcPreview.left + (long)(GetItemHeight(lpDrawItemStruct->itemID) * PREVIEW_RATIO);
			DrawPreviewImage(pDC, strFilePath, rcPreview);
		}
	}
#endif
}
HBITMAP CSearchComboCtrl::ReadDWGPreviewImageData(CDC *pDC,CString csFileName)
{
	unsigned char ucSpecialCode[16];
	//Ԥ��ͼƬ�����ֶ�16�ֽ�
	//unsigned char ucNotSpecialCode[16];//����16�ֽ�
	unsigned long lFileOffset;		//�ļ�ָ��ƫ�ƣ�
	unsigned long lAllPreviewSize;	//Ԥ��ͼƬ��С
	BYTE iBlockNumber;		//Ԥ��ͼƬ����
	BYTE DB_Type;			//ͼƬ����
	unsigned long DB_Offset;//ͼƬ����ʵλ��
	unsigned long DB_Size;	//ͼƬ�Ĵ�С
	CFile file;
	long i;
	if(!file.Open(csFileName,CFile::modeRead|CFile::shareDenyNone,NULL)) return NULL;//��DWG�ļ�����ʹ��ʱ������ʹ��CFile::shareDenyNone������ȷ��
	file.Seek(13L,CFile::begin);//�ļ�ָ�����ڵ�13�ֽڴ�
	file.Read(&lFileOffset,sizeof(long));//��ȡԤ��ͼƬλ��
	file.Seek(lFileOffset,CFile::begin);//�����ļ�ָ��,��ȡԤ��ͼƬ�����ֶ�
	file.Read(ucSpecialCode,16*sizeof(unsigned char));
	if(ucSpecialCode[0]!=0x1F||ucSpecialCode[1]!=0x25||
		ucSpecialCode[2]!=0x6D||ucSpecialCode[3]!=0x07||
		ucSpecialCode[4]!=0xD4||ucSpecialCode[5]!=0x36||
		ucSpecialCode[6]!=0x28||ucSpecialCode[7]!=0x28||
		ucSpecialCode[8]!=0x9D||ucSpecialCode[9]!=0x57||
		ucSpecialCode[10]!=0xCA||ucSpecialCode[11]!=0x3F||
		ucSpecialCode[12]!=0x9D||ucSpecialCode[13]!=0x44||
		ucSpecialCode[14]!=0x10||ucSpecialCode[15]!=0x2B)
	{
		return NULL;//Ԥ��ͼƬ�������ֶδ��󣬷���;
	}
	file.Read(&lAllPreviewSize,sizeof(long));

	//�����ļ�ָ��,�ص����ݶ�	
	file.Read(&iBlockNumber,sizeof(BYTE));//��ȡ�ļ�Ԥ��ͼƬ����
	for(i=0;i<iBlockNumber;i++)
	{
		file.Read(&DB_Type,sizeof(BYTE));
		file.Read(&DB_Offset,sizeof(long));
		file.Read(&DB_Size,sizeof(long));
		if(DB_Type==0x2)break;//Ԥ��ͼƬ����ΪBMPͼ���˳�
	}
	if(i==iBlockNumber) return NULL;
	file.Seek(DB_Offset,CFile::begin);//����BMPͼƬ������
	BYTE *pDib=new BYTE[DB_Size];//����ͼƬ��С�洢�ռ�
	if(pDib==NULL) return NULL;
	file.Read(pDib,DB_Size);//��ȡBMPͼƬ����
	file.Close();

	LPBITMAPINFO m_lpBmpInfo;
	LPBITMAPINFOHEADER m_lpBmpInfoHeader;
	LPRGBQUAD m_lpColorTable;//�߼���ɫ��ָ��
	LPBYTE m_lpImageData;	//ͼ������ָ��
	int m_ColorNumber=256;	//��ɫ��Ϊ256ɫ
	m_lpBmpInfo=(LPBITMAPINFO)pDib;
	m_lpBmpInfoHeader=(LPBITMAPINFOHEADER)pDib;
	m_lpColorTable=(LPRGBQUAD)(pDib+m_lpBmpInfoHeader->biSize);
	m_lpImageData=(LPBYTE)m_lpColorTable+m_ColorNumber*sizeof(RGBQUAD);
	int bmpWidth=m_lpBmpInfoHeader->biWidth;	//ͼ����
	int bmpHeight=m_lpBmpInfoHeader->biHeight;	//ͼ��߶�
	//���������߼���ɫ��
	LPRGBQUAD pDibQuad;
	HPALETTE hPal,hOldPal;
	LPLOGPALETTE pLogPal=(LPLOGPALETTE)new char[2*sizeof(WORD)+m_ColorNumber*sizeof(PALETTEENTRY)];
	pLogPal->palVersion=0x300;
	pLogPal->palNumEntries=m_ColorNumber;
	pDibQuad=(LPRGBQUAD)m_lpColorTable;
	for(i=0;i<m_ColorNumber;i++)
	{
		pLogPal->palPalEntry[i].peRed=pDibQuad->rgbRed;
		pLogPal->palPalEntry[i].peGreen=pDibQuad->rgbGreen;
		pLogPal->palPalEntry[i].peBlue=pDibQuad->rgbBlue;
		pLogPal->palPalEntry[i].peFlags=0;
		pDibQuad++;
	}

	hPal=::CreatePalette((LPLOGPALETTE)pLogPal);
	hOldPal=::SelectPalette(pDC->m_hDC,hPal,FALSE);
	::RealizePalette(pDC->m_hDC);

	int offx,offy;//ƫ��ֵ,ʹͼ�������ʾ
	offx=0;
	offy=0;
	if(offx<0) offx=0;//����ť���С��BMP��ȣ�x�᲻ƫ��
	if(offy<0) offy=0;//����ť�߶�С��BMP�߶ȣ�y�᲻ƫ��

	HBITMAP hDeskBitmap = ::CreateCompatibleBitmap(pDC->GetSafeHdc(),bmpWidth,bmpHeight);
	CDC cdMem; cdMem.CreateCompatibleDC(pDC);
	HBITMAP pOldBitmap = (HBITMAP)::SelectObject(cdMem.m_hDC,hDeskBitmap);
	//cdMem.SelectStockObject(BLACK_BRUSH);
	cdMem.SelectStockObject(WHITE_BRUSH);
	cdMem.FillRect(CRect(0,0,bmpWidth,bmpHeight),cdMem.GetCurrentBrush());

	StretchDIBits(cdMem.GetSafeHdc(),offx,offy,bmpWidth,bmpHeight,
		0,0,bmpWidth,bmpHeight,(void*)m_lpImageData,m_lpBmpInfo,
		DIB_RGB_COLORS,SRCAND);//ȡ��ԭͼ
	::SelectPalette(pDC->m_hDC,hOldPal,FALSE);
	::DeleteObject(hPal);
	delete [] pDib;
	delete [] pLogPal;
	cdMem.SelectObject(pOldBitmap);
	cdMem.DeleteDC();
	return hDeskBitmap;
}

void CSearchComboCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	int nIndex = GetCurSel();
	if (!GetDroppedState() && nIndex != CB_ERR)
	{
		CClientDC dc(this);
		TEXTMETRIC metrics;
		GetTextMetrics(dc, &metrics);

		CRect rc;
		GetClientRect(&rc);

		rc.right = metrics.tmHeight + metrics.tmExternalLeading + 6;

		if (rc.PtInRect(point))
		{
			if (GetFocus() != this)
				SetFocus();
			SetCheck(nIndex, !GetCheck(nIndex));
			GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);
			return;
		}
	}
	CComboBox::OnLButtonDown(nFlags, point);
}

BOOL CSearchComboCtrl::OnDropDown() 
{
	m_pComboBox = this;
	return FALSE; // Parent window will also receive this notification
}

LRESULT CSearchComboCtrl::OnCtlColorListBox(WPARAM, LPARAM lParam) 
{
#ifdef PREVIEW_SHOW
	// If the listbox hasn't been subclassed yet, do so...
	if (m_hListBox == NULL)
	{
		HWND hWnd = reinterpret_cast<HWND>(lParam);

		if (hWnd != 0 && hWnd != m_hWnd)
		{
			// Save the listbox handle
			m_hListBox = hWnd;
			// Do the subclassing
			m_pWndProc = reinterpret_cast<WNDPROC>(GetWindowLong(m_hListBox, GWL_WNDPROC));
			SetWindowLong(m_hListBox, GWL_WNDPROC, reinterpret_cast<long>(ComboBoxListBoxProc));
		}
	}
#endif
	return Default();
}
LRESULT CSearchComboCtrl::OnAddString(WPARAM, LPARAM)
{
	int nIndex = (int)Default();
	if (nIndex >= 0)
	{
		m_CheckArray.InsertAt(nIndex, 0, 1);
		m_PreviewArray.InsertAt(nIndex, 0, 1);
	}
	return nIndex;
}
LRESULT CSearchComboCtrl::OnDeleteString(WPARAM wp, LPARAM)
{
	int nCount = (int)Default();
	if (nCount != CB_ERR)
	{
		m_CheckArray.RemoveAt(wp);
		m_PreviewArray.RemoveAt(wp);
	}
	return nCount;
}

LRESULT CSearchComboCtrl::OnResetContent(WPARAM, LPARAM)
{
	m_CheckArray.RemoveAll();
	m_PreviewArray.RemoveAll();
	return Default();
}

LRESULT CSearchComboCtrl::OnEditChange(WPARAM wParam, LPARAM lParam)
{
	EditCtrlChanged();
	return 0;
}

BOOL CSearchComboCtrl::SetCheck(int nIndex, BOOL bCheck)
{
	BOOL ret = GetCheck(nIndex);
	if (ret != bCheck)
	{
		m_CheckArray.SetAt(nIndex, (BYTE)bCheck);
		// Redraw the window
		Invalidate(FALSE);
	}
	return ret;
}
BOOL CSearchComboCtrl::SetPreview(int nIndex, BOOL bPreview)
{
	BOOL ret = GetPreview(nIndex);
	if (ret != bPreview)
	{
		m_PreviewArray.SetAt(nIndex, (BYTE)bPreview);
		// Redraw the window
		Invalidate(FALSE);
	}
	return ret;
}

void CSearchComboCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_SPACE && !GetDroppedState() && (nFlags & 1 << 14) == 0)
	{
		int nIndex = GetCurSel();
		if (nIndex != CB_ERR)
		{
			SetCheck(nIndex, !GetCheck(nIndex));
			GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);
			return;
		}
	}
	if (nChar == VK_ESCAPE)
		return;
	CComboBox::OnChar(nChar, nRepCnt, nFlags);
}

void CSearchComboCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_SPACE)
		return;

	CComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CSearchComboCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	OnLButtonDown(nFlags, point);
}

void CSearchComboCtrl::OnTimer(UINT nIDEvent)
{
	switch (nIDEvent)
	{
	case 1:
		if (GetDroppedState( ))
		{
			static CString strFilePath;
			static int index = 0;
			index = 0;
			static int cursel = 0;
			cursel = GetCurSel();
			if (m_bHistory)
			{
				for (set<pair<string, string> >::iterator iter = m_UserSearchHistory.begin();
					iter != m_UserSearchHistory.end(); ++iter)
				{
					if (index ++ == cursel)
					{
						strFilePath = iter->second.c_str();
						break;
					}
				}
			}
			else
			{
				for (list<pair<string, string> >::iterator iter = m_Results.begin();  iter != m_Results.end(); ++iter)
				{
					if (index ++ == cursel)
					{
						strFilePath = iter->second.c_str();
						break;
					}
				}
			}
			if (cursel != -1)
			{
				CRect rc;
				GetDroppedControlRect(rc);
				int nHeight = GetItemHeight(0) * ((cursel - GetTopIndex()) + 1);
				CPoint pt(rc.right + 5,rc.top + nHeight);
				m_wndTip.ShowTips(pt,strFilePath);
			}
		}
		KillTimer(1);
		break;
	default:
		break;
	}
	CComboBox::OnTimer(nIDEvent);
}

void CSearchComboCtrl::OnCloseUp()
{
	size_t count = GetCurSel(), index = 0;
	for (list<pair<string, string> >::iterator iter = m_Results.begin(); iter != m_Results.end(); ++iter)
	{
		if (count == index++)
		{
			m_UserSearchHistory.insert(*iter);
			break;
		}
	}
	m_wndTip.ShowWindow(SW_HIDE);
}
void CSearchComboCtrl::RecalcDropWidth()
{
	// Reset the dropped width
	int nNumEntries = GetCount();
	int nWidth = 0;
	CString str;

	CClientDC dc(this);
	int nSave = dc.SaveDC();
	dc.SelectObject(GetFont());

	int nScrollWidth = ::GetSystemMetrics(SM_CXVSCROLL);
	for (int i = 0; i < nNumEntries; i++)
	{
		GetLBText(i, str);
		int nLength = dc.GetTextExtent(str).cx + nScrollWidth;
		nWidth = max(nWidth, nLength);
	}
	// Add margin space to the calculations
	nWidth += dc.GetTextExtent("00").cx;
#ifdef PREVIEW_SHOW
	if (GetCount() > 0)
		 nWidth += 20 + (int)(GetItemHeight(0) * PREVIEW_RATIO);
#endif
	CRect rc;
	GetClientRect(&rc);
	if (rc.Width() << 1 < nWidth)//���ȡԭ��Ͽ��ȵ�2��
		nWidth = rc.Width() << 1;
	dc.RestoreDC(nSave);
	SetDroppedWidth(nWidth);
}
BOOL CSearchComboCtrl::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_SPACE || pMsg->wParam == VK_RETURN)
			return 0;
		break;
	default:
		break;
	}
	return CComboBox::PreTranslateMessage(pMsg);
}
//��ȡ�û���ʷ������¼
bool CSearchComboCtrl::ReadUserSearchHisroty()
{
	if (!m_bHistory)
		return false;
	char path[MAX_PATH];
	DWORD ret = GetModuleFileName(NULL, path, MAX_PATH);
	if (!ret)
		return false;
	string strPath = path, filename;
	strPath = strPath.substr(0, strPath.find_last_of('\\'));
	strPath += "\\";
	strPath += SEARCHHISTORYFILENAME;
	m_UserSearchHistory.clear();
	fstream fs;
	fs.open(strPath.c_str(), fstream::in);
	fs.getline(path, MAX_PATH);
	typedef pair<string, string> sspair;
	while (fs.good())
	{
		strPath = path;
		filename = strPath.substr(strPath.find_last_of('\\') + 1);
		m_UserSearchHistory.insert(sspair(filename, strPath));
		fs.getline(path, MAX_PATH);
	}
	fs.close();
	return true;
}
//���µ��û���¼д�̣�����������
bool CSearchComboCtrl::WriteUserSearchhistory()
{
	char path[MAX_PATH];
	DWORD ret = GetModuleFileName(NULL, path, MAX_PATH);
	if (!ret)
		return false;
	string strPath = path;
	strPath = strPath.substr(0, strPath.find_last_of('\\'));
	strPath += "\\";
	strPath += SEARCHHISTORYFILENAME;
	ofstream fs;
	fs.open(strPath.c_str(), fstream::out);
	for (set<pair<string, string> >::iterator iter = m_UserSearchHistory.begin(); iter != m_UserSearchHistory.end(); ++iter)
	{
		fs.write(iter->second.c_str(), (std::streamsize)(iter->second.length()));
		fs.put('\n');
	}
	fs.close();
	return true;
}
void CSearchComboCtrl::OnCbnSetfocus()
{
	CString str;
	GetWindowText(str);
	if (str.IsEmpty())
		PostMessage(WM_RESETLISTBOX, 0, 0);
}

void CSearchComboCtrl::OnDestroy()
{
	CComboBox::OnDestroy();
	WriteUserSearchhistory();
}