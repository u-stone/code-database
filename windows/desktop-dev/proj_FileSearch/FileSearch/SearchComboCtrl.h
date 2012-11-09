#pragma once
#include "afxwin.h"
#include "FileSearcher.h"
#include "TipWnd.h"
#include <iostream>
using namespace std;
using namespace FileSearch;

//#define CHECK_SHOW   //控制是否显示多选框
//#define PREVIEW_SHOW	//控制是否显示预览图
#define MAX_SEARCHNUM 50		//定义最大的用户历史搜索记录数目
#define  SEARCHHISTORYFILENAME		"SearchHistory.txt"

#define  WM_RESETLISTBOX (WM_USER + 2)
#define  WM_EDITCHANGE	 (WM_USER + 3)
#define	 PREVIEW_RATIO 1.56
//重载组合框，添加界面修饰
class CSearchComboCtrl : public CComboBox
{
public:
	CSearchComboCtrl(void);
	~CSearchComboCtrl(void);
private:
	bool m_bHistory;//标记当前下拉列表中的内容是搜索历史true，还是搜索结果false
	volatile bool m_bDelete;
	HWND m_hListBox;
	CTipWnd m_wndTip;
	list<pair<string, string> > m_Results;
	set<pair<string, string> > m_UserSearchHistory;//记录用户搜索历史
	CFileSearcher m_DWGFileSearcher;
public:
	void ToSearch();//准备接收搜索字符串
	bool BuildIndex();
	bool AddSearchFolder(string pAddedFolder);
	bool GetResult(list<pair<string, string> >& sFilesSet);
	string GetFilePathSelected();
public:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT ResetListBox(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCbnEditchange();
	//afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
protected:
	afx_msg LRESULT OnCtlColorListBox(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEditChange(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnDropDown();
	afx_msg void OnCloseUp();
	afx_msg LRESULT OnAddString(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnDeleteString(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnResetContent(WPARAM wp, LPARAM lp);
public:
	//复选按钮
	BOOL GetCheck(int nIndex){
		return (m_CheckArray.GetAt(nIndex) ? TRUE : FALSE);
	}
	// Selects/unselects the specified item
	BOOL SetCheck(int nIndex, BOOL bCheck = TRUE);

public:
	//预览
	BOOL GetPreview(int index){
		return m_PreviewArray[index];
	}
	BOOL SetPreview(int index, BOOL bPreview = TRUE);
private:
	CByteArray m_CheckArray;
	CByteArray m_PreviewArray;
	void DrawCheckbtn(HDC hdc, LPDRAWITEMSTRUCT lpDrawItemStruct);
	void DrawPreview(CDC* pDC, LPDRAWITEMSTRUCT lpDrawItemStruct);//控制预览
	//读取DWG文件图片预览文件
	HBITMAP ReadDWGPreviewImageData(CDC *pDC,CString csFileName);
	void DrawPreviewImage(CDC * pDC, CString csFileName, RECT rect);
	void RecalcDropWidth();
	//读取用户历史搜索记录
	bool ReadUserSearchHisroty();
	//将新的用户记录写盘，并做好排序
	bool WriteUserSearchhistory();
	//编辑框中的内容改变的时候应做的操作
	void EditCtrlChanged();
	//绘制编辑框外观
	void DrawEdit(CDC* pDC, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//绘制下拉列表外观
	void DrawListItem(CDC* pDC, LPDRAWITEMSTRUCT lpDrawItemStruct);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnCbnSetfocus();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
};
