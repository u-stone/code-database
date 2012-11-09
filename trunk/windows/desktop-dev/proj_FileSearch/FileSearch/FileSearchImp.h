#pragma once
#include "ifilesearch.h"
#include "SearchComboCtrl.h"
#include "SearchDlg.h"


class CFileSearchImp : public IFileSearch
{
public:
	CFileSearchImp(void);
	CFileSearchImp(CWnd* pParentWnd);
	virtual ~CFileSearchImp(void);
public:
	virtual void InputStr(string strSearch);//输入要搜索的文件名字符前缀
	virtual bool Select(int index);//选中搜索结果项--
	virtual bool AddSearchFolder(string pAddedFolder);//添加搜索的文件夹
	virtual bool ExcludeSearchFolder(string pExcludeFolder);//添加不搜索的文件夹
	virtual bool GetResults(list<pair<string, string> >& sFilesSet);//返回搜索结果--
	virtual HWND GetHWnd();//获得组合框的句柄
	virtual void ToSearch();//准备接收搜索字符串
	virtual string GetSelectedFilePath();//获得选中的文件路径字符串--
private:
	//创建组合框
	void CreateComboBox(CWnd* pParentWnd);
	HWND m_hWnd;
	CSearchDlg m_SearchDlg;
	//CSearchComboCtrl m_ComboBox;
};