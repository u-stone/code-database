
// FTPDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include <afxinet.h>
#include "afxwin.h"


// CFTPDlg 对话框
class CFTPDlg : public CDialog
{
// 构造
public:
	CFTPDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_FTP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
private:
	CString m_strIP;
	CString m_strPort;
	CString m_strUser;
	CString m_strPw;
	BOOL m_bConnect;
	CInternetSession * m_pInternetSession;
	CFtpConnection * m_pFtpConnection;

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnconnect();
	CListBox m_FtpFileListBox;
	afx_msg void OnBnClickedBtnupload();
	afx_msg void OnBnClickedChknoname();
	CButton m_NoName;
};
