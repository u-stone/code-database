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
	virtual void InputStr(string strSearch);//����Ҫ�������ļ����ַ�ǰ׺
	virtual bool Select(int index);//ѡ�����������--
	virtual bool AddSearchFolder(string pAddedFolder);//����������ļ���
	virtual bool ExcludeSearchFolder(string pExcludeFolder);//��Ӳ��������ļ���
	virtual bool GetResults(list<pair<string, string> >& sFilesSet);//�����������--
	virtual HWND GetHWnd();//�����Ͽ�ľ��
	virtual void ToSearch();//׼�����������ַ���
	virtual string GetSelectedFilePath();//���ѡ�е��ļ�·���ַ���--
private:
	//������Ͽ�
	void CreateComboBox(CWnd* pParentWnd);
	HWND m_hWnd;
	CSearchDlg m_SearchDlg;
	//CSearchComboCtrl m_ComboBox;
};