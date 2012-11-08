
// ProcTimerDlg.h : 头文件
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



// CProcTimerDlg 对话框
class CProcTimerDlg : public CDialog
{
// 构造
public:
	CProcTimerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_PROCTIMER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	SYSTEMTIME m_st;
	int m_Method;
	LONG m_Period;
	NOTIFYICONDATA m_traydata;
	CMenu m_menu;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg LRESULT OnTrayMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


private:
	// 推出之前的操作
	void PreExit();
	// 获得系统时间
	void GetUserSystemTime();
	// 得到程序当前运行的路径
	CString getInitFilePath();
	// 读取配置文件
	bool readInitData();
	// 写入配置文件
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

