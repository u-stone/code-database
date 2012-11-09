
// ProcTimerDlg.h : ͷ�ļ�
//

#pragma once
#include "afxdtctl.h"
#include "afxwin.h"

#define SEC		0
#define MIN		1
#define HOUR	2
#define DAY		3
#define MON		4
#define YEAR	5

#define TSEC	1000
#define TMINUTE ((TSEC)*60)
#define THOUR ((TMINUTE)*60)
#define TDAY ((THOUR)*24)
#define TMON ((TDAY)*30)
#define TYEAR ((TMON)*12)



// CProcTimerDlg �Ի���
class CProcTimerDlg : public CDialog
{
// ����
public:
	CProcTimerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_PROCTIMER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	SYSTEMTIME m_st;
	int m_Method;
	LONG m_Period;
	NOTIFYICONDATA m_traydata;
	CMenu m_menu;
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg LRESULT OnTrayMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


private:
	// �Ƴ�֮ǰ�Ĳ���
	void PreExit();
	// ���ϵͳʱ��
	void GetUserSystemTime();
	// �õ�����ǰ���е�·��
	CString getInitFilePath();
	// ��ȡ�����ļ�
	bool readInitData();
	// д�������ļ�
	void writeInitData();

public:
	afx_msg void OnBnClickedBtnproc();
	CComboBox m_CmbFrequency;
	afx_msg void OnBnClickedBtnexecute();
	afx_msg void OnCbnSelchangeCmbmethod();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
	afx_msg void OnShowMainUi();
	afx_msg void OnExitApp();
	afx_msg void OnBnClickedRadiosingle();
	afx_msg void OnBnClickedRadiomulti();
	afx_msg void OnBnClickedBtnquit();
};

