
// FileMonDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "WinFileMon.h"
#include <list>
#include "FtpConnecter.h"

// CFileMonDlg 对话框
class CFileMonDlg : public CDialog
{
// 构造
public:
	CFileMonDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_FILEMON_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
private:
	WinFileMon m_FileMon;
	std::vector<CString> m_Dirs;
	CString m_ConvertProcPath;
	CString m_strFtpServerIP;
	CString m_strFtpServerPort;
	CString m_strFtpUser;
	CString m_strFtpPassword;
	FtpConnecter m_FtpConnecter;
	CRect m_ClientRect;
	CRect m_OldClientRect;
	bool	m_bShowConsole;
private:
	void ReadInit();
	void WriteInit();
	void PreExit();
// 实现
protected:
	HICON m_hIcon;
	NOTIFYICONDATA m_traydata;
	CMenu m_menu;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnTrayMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateRemoteFolderPath(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateLocalFolderPath(WPARAM wParam, LPARAM lParam);
	afx_msg void OnShowMainUi();
	afx_msg void OnExitApp();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnmonitoring();
	afx_msg void OnBnClickedBtnaddpath();
	afx_msg void OnBnClickedBtndelpath();
	CListBox m_PathListCtrl;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnopenconvertproc();
	afx_msg void OnBnClickedBtnexeconvertproc();
	afx_msg void OnBnClickedBtntryconnect();
	afx_msg void OnBnClickedChkshowpw();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnflushlog();
	afx_msg void OnBnClickedBtnswitchconsole();
	afx_msg void OnClose();
};
