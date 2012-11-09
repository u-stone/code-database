#include "StdAfx.h"
#include ".\filesearchimp.h"
#include "SearchDlg.h"

CFileSearchImp::CFileSearchImp(void)
{
	m_hWnd = NULL;
}
CFileSearchImp::CFileSearchImp(CWnd* pParentWnd)
{
	m_hWnd = NULL;
	CreateComboBox(pParentWnd);
}
CFileSearchImp::~CFileSearchImp(void)
{
	//DestroyWindow(m_hWnd);
}
//输入要搜索的文件名字符前缀
void CFileSearchImp::InputStr(string strSearch)
{
	if (m_SearchDlg.m_ComboCtrl.GetSafeHwnd() != NULL)
	{
		SetWindowText(m_SearchDlg.m_ComboCtrl.GetSafeHwnd(), strSearch.c_str());
		::PostMessage(m_SearchDlg.m_ComboCtrl.GetSafeHwnd(), WM_EDITCHANGE, 0, 0);
	}
}
//选中搜索结果项
bool CFileSearchImp::Select(int index)
{
	if (m_SearchDlg.m_ComboCtrl.GetCount() > index)
		m_SearchDlg.m_ComboCtrl.SetCurSel(index);
	return true;
}
//添加搜索的文件夹
bool CFileSearchImp::AddSearchFolder(string pAddedFolder)
{
	return m_SearchDlg.m_ComboCtrl.AddSearchFolder(pAddedFolder);
}
//添加不搜索的文件夹
bool CFileSearchImp::ExcludeSearchFolder(string pExcludeFolder)
{
	//TODO:暂时不做
	return true;
}
//返回搜索结果
bool CFileSearchImp::GetResults(list<pair<string, string> >& sFilesSet)
{
	return m_SearchDlg.m_ComboCtrl.GetResult(sFilesSet);
}
//创建对话框线程
void CFileSearchImp::CreateComboBox(CWnd* pParentWnd)
{
	//创建搜索框
	//if (pParentWnd != NULL)
	//{
	//	m_ComboBox.Create(WS_CHILD|WS_VISIBLE|WS_VSCROLL | WS_HSCROLL |CBS_AUTOHSCROLL|CBS_DROPDOWN | CBS_HASSTRINGS, 
	//		CRect(50, 50, 200, 200), pParentWnd, 1);
	//	m_ComboBox.ShowWindow(SW_SHOW);
	//	m_hWnd = m_ComboBox.GetSafeHwnd();
	//}
	//else
	//{
	m_SearchDlg.Create(CSearchDlg::IDD, pParentWnd);
	//m_SearchDlg.GetDlgItem(IDC_ComboSearch,&m_hWnd);
	m_SearchDlg.ShowWindow(SW_SHOW);
	m_hWnd = m_SearchDlg.GetSafeHwnd();
	//m_SearchDlg.m_ComboCtrl.SetOwner(&m_SearchDlg);
	//SetParent(m_hWnd, pParentWnd->GetSafeHwnd());
	//LONG ws = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
	//::SetWindowLong(m_hWnd, GWL_EXSTYLE, ws & ~WS_EX_TOPMOST);
	//}
	//添加皮肤
	//BOOL b = dsSkinWindow(m_hWnd, SKIN_TYPE_COMBOBOX, _T("FileSearch"), TRUE);
}

HWND CFileSearchImp::GetHWnd()
{
	return m_SearchDlg.GetSafeHwnd();
}

void CFileSearchImp::ToSearch()
{
	m_SearchDlg.m_ComboCtrl.BuildIndex();
	m_SearchDlg.m_ComboCtrl.ToSearch();
}

string CFileSearchImp::GetSelectedFilePath()
{
	return m_SearchDlg.m_ComboCtrl.GetFilePathSelected();
}

extern "C" __declspec(dllexport) IFileSearch* GetFileSearcher(CWnd* pParentWnd)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return new CFileSearchImp(pParentWnd);
}