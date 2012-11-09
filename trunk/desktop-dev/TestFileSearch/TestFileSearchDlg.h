// TestFileSearchDlg.h : ͷ�ļ�
//

#pragma once
#include "..\FileSearch\IFileSearch.h"


// CTestFileSearchDlg �Ի���
class CTestFileSearchDlg : public CDialog
{
// ����
public:
	CTestFileSearchDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TESTFILESEARCH_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	IFileSearch* m_pFileSearch;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
protected:
	virtual void OnOK();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedbtnsubmit();
	afx_msg void OnBnClickedbtnselect();
	afx_msg void OnBnClickedbtngetpath();
};
