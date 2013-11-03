
// SimplePlayerDlg.h : ͷ�ļ�
//

#pragma once
#include "WavePlayer.h"


// CSimplePlayerDlg �Ի���
class CSimplePlayerDlg : public CDialogEx
{
// ����
public:
	CSimplePlayerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SIMPLEPLAYER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


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
