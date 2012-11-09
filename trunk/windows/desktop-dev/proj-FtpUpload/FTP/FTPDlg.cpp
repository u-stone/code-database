
// FTPDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FTP.h"
#include "FTPDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CFTPDlg 对话框




CFTPDlg::CFTPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFTPDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strIP = _T("192.168.111.132");
	m_strPort = _T("21");
	//m_strUser = _T("lgy047");
	//m_strPw = _T("0512108587");
}

void CFTPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LstFileBox, m_FtpFileListBox);
	DDX_Control(pDX, IDC_ChkNoName, m_NoName);
}

BEGIN_MESSAGE_MAP(CFTPDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BtnConnect, &CFTPDlg::OnBnClickedBtnconnect)
	ON_BN_CLICKED(IDC_BtnUpload, &CFTPDlg::OnBnClickedBtnupload)
	ON_BN_CLICKED(IDC_ChkNoName, &CFTPDlg::OnBnClickedChknoname)
END_MESSAGE_MAP()


// CFTPDlg 消息处理程序

BOOL CFTPDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_bConnect = FALSE;
	GetDlgItem(IDC_EdtIP)->SetWindowText(m_strIP);
	GetDlgItem(IDC_EdtPort)->SetWindowText(m_strPort);
	GetDlgItem(IDC_EdtUserName)->SetWindowText(m_strUser);
	GetDlgItem(IDC_EdtPW)->SetWindowText(m_strPw);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CFTPDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFTPDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CFTPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CFTPDlg::OnBnClickedBtnconnect()
{
	GetDlgItem(IDC_EdtIP)->GetWindowText(m_strIP);
	GetDlgItem(IDC_EdtPort)->GetWindowText(m_strPort);
	GetDlgItem(IDC_EdtUserName)->GetWindowText(m_strUser);
	GetDlgItem(IDC_EdtPW)->GetWindowText(m_strPw);
	if (m_strIP.IsEmpty() || m_strPort.IsEmpty() || m_strUser.IsEmpty())
		return ;
	m_pInternetSession = new CInternetSession(_T("MR"), INTERNET_OPEN_TYPE_PRECONFIG);
	try
	{
		m_pFtpConnection = m_pInternetSession->GetFtpConnection(m_strIP, m_strUser, m_strPw, atoi(m_strPort.GetBuffer()), TRUE);
		m_strPort.ReleaseBuffer();
		m_bConnect = TRUE;
	}
	catch (CInternetException* pEx)
	{
		TCHAR szErr[1024];
		pEx->GetErrorMessage(szErr, 1024);
		AfxMessageBox(szErr);
		pEx->Delete();
	}
	m_FtpFileListBox.ResetContent();
	CFtpFileFind ftpfind(m_pFtpConnection);
	BOOL bfind = ftpfind.FindFile(NULL);
	while (bfind)
	{
		bfind = ftpfind.FindNextFile();
		CString strpath = ftpfind.GetFileURL();
		m_FtpFileListBox.AddString(strpath);
	}
}

void CFTPDlg::OnBnClickedBtnupload()
{
	CString str;
	CString strname;
	CFileDialog file(true, _T("file"), NULL, 4|2, _T("所有文件|*.*||"), this);
	if (file.DoModal() == IDOK)
	{
		str = file.GetPathName();
		strname = file.GetFileName();
	}
	if (m_bConnect)
	{
		BOOL bput = m_pFtpConnection->PutFile((LPCTSTR)str, (LPCTSTR)strname);
		if (bput)
		{
			m_FtpFileListBox.ResetContent();
			this->OnBnClickedBtnconnect();
			AfxMessageBox(_T("上传成功"));
		}
	}
}

void CFTPDlg::OnBnClickedChknoname()
{
	int icheck = m_NoName.GetCheck();
	if (icheck == 1)
	{
		GetDlgItem(IDC_EdtUserName)->EnableWindow(FALSE);
		GetDlgItem(IDC_EdtPW)->EnableWindow(FALSE);
		GetDlgItem(IDC_EdtUserName)->SetWindowText(_T("anonynous"));
		GetDlgItem(IDC_EdtPW)->SetWindowText(_T(""));
	}
	else
	{
		GetDlgItem(IDC_EdtUserName)->EnableWindow(TRUE);
		GetDlgItem(IDC_EdtPW)->EnableWindow(TRUE);
		GetDlgItem(IDC_EdtUserName)->SetWindowText(_T(""));
		GetDlgItem(IDC_EdtPW)->SetWindowText(_T(""));
	}
}
