
// SimplePlayerDlg.h : 头文件
//

#pragma once
#include "WavePlayer.h"


// CSimplePlayerDlg 对话框
class CSimplePlayerDlg : public CDialogEx
{
// 构造
public:
	CSimplePlayerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SIMPLEPLAYER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


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
	afx_msg void OnBnClickedBtnopenfile();
	afx_msg void OnBnClickedBtnplay();

private:
	void InitMember();
	BOOL InitDS();
	BOOL UninitDS();
	BOOL CreatePrimaryDSB();
	BOOL m_bInit;
	LPDIRECTSOUND m_pIDS;
	LPDIRECTSOUNDBUFFER m_pPrimaryBuffer;
	HWND m_hPlayer;
	WavePlayer *m_pPalyer;
public:
	afx_msg void OnClose();
	afx_msg void OnBnClickedBtbstop();
	afx_msg void OnBnClickedBtnrestart();
};
