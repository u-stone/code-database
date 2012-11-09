
// FTPDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include <afxinet.h>
#include "afxwin.h"


// CFTPDlg �Ի���
class CFTPDlg : public CDialog
{
// ����
public:
	CFTPDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_FTP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
private:
	CString m_strIP;
	CString m_strPort;
	CString m_strUser;
	CString m_strPw;
	BOOL m_bConnect;
	CInternetSession * m_pInternetSession;
	CFtpConnection * m_pFtpConnection;

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
