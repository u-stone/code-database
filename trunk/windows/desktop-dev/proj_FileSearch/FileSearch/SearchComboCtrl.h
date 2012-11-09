#pragma once
#include "afxwin.h"
#include "FileSearcher.h"
#include "TipWnd.h"
#include <iostream>
using namespace std;
using namespace FileSearch;

//#define CHECK_SHOW   //�����Ƿ���ʾ��ѡ��
//#define PREVIEW_SHOW	//�����Ƿ���ʾԤ��ͼ
#define MAX_SEARCHNUM 50		//���������û���ʷ������¼��Ŀ
#define  SEARCHHISTORYFILENAME		"SearchHistory.txt"

#define  WM_RESETLISTBOX (WM_USER + 2)
#define  WM_EDITCHANGE	 (WM_USER + 3)
#define	 PREVIEW_RATIO 1.56
//������Ͽ���ӽ�������
class CSearchComboCtrl : public CComboBox
{
public:
	CSearchComboCtrl(void);
	~CSearchComboCtrl(void);
private:
	bool m_bHistory;//��ǵ�ǰ�����б��е�������������ʷtrue�������������false
	volatile bool m_bDelete;
	HWND m_hListBox;
	CTipWnd m_wndTip;
	list<pair<string, string> > m_Results;
	set<pair<string, string> > m_UserSearchHistory;//��¼�û�������ʷ
	CFileSearcher m_DWGFileSearcher;
public:
	void ToSearch();//׼�����������ַ���
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
	//��ѡ��ť
	BOOL GetCheck(int nIndex){
		return (m_CheckArray.GetAt(nIndex) ? TRUE : FALSE);
	}
	// Selects/unselects the specified item
	BOOL SetCheck(int nIndex, BOOL bCheck = TRUE);

public:
	//Ԥ��
	BOOL GetPreview(int index){
		return m_PreviewArray[index];
	}
	BOOL SetPreview(int index, BOOL bPreview = TRUE);
private:
	CByteArray m_CheckArray;
	CByteArray m_PreviewArray;
	void DrawCheckbtn(HDC hdc, LPDRAWITEMSTRUCT lpDrawItemStruct);
	void DrawPreview(CDC* pDC, LPDRAWITEMSTRUCT lpDrawItemStruct);//����Ԥ��
	//��ȡDWG�ļ�ͼƬԤ���ļ�
	HBITMAP ReadDWGPreviewImageData(CDC *pDC,CString csFileName);
	void DrawPreviewImage(CDC * pDC, CString csFileName, RECT rect);
	void RecalcDropWidth();
	//��ȡ�û���ʷ������¼
	bool ReadUserSearchHisroty();
	//���µ��û���¼д�̣�����������
	bool WriteUserSearchhistory();
	//�༭���е����ݸı��ʱ��Ӧ���Ĳ���
	void EditCtrlChanged();
	//���Ʊ༭�����
	void DrawEdit(CDC* pDC, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//���������б����
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
