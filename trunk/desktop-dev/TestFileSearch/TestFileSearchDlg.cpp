// TestFileSearchDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestFileSearch.h"
#include "TestFileSearchDlg.h"
#include ".\testfilesearchdlg.h"

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


// CTestFileSearchDlg 对话框



CTestFileSearchDlg::CTestFileSearchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestFileSearchDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestFileSearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestFileSearchDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_btnSubmit, OnBnClickedbtnsubmit)
	ON_BN_CLICKED(IDC_btnSelect, OnBnClickedbtnselect)
	ON_BN_CLICKED(IDC_btnGetPath, OnBnClickedbtngetpath)
END_MESSAGE_MAP()


// CTestFileSearchDlg 消息处理程序

BOOL CTestFileSearchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将\“关于...\”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
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
	HMODULE hModule = LoadLibrary(_T("FileSearch.Dll"));
	if (hModule != NULL)
	{
		typedef IFileSearch* (FileSearchFunc)(CWnd*) ;
		FileSearchFunc* func = (FileSearchFunc*)GetProcAddress(hModule, _T("GetFileSearcher"));
//#define N
//#ifdef	N
		m_pFileSearch = func(this);//作为父窗口，只创建组合框
//#else
//		m_pFileSearch = func(NULL);
//		if (m_pFileSearch != NULL)
//		{
//			HWND hWnd = ::GetParent(m_pFileSearch->GetHWnd());
//			::SetParent(m_pFileSearch->GetHWnd(), GetSafeHwnd());
//		} 
//#endif
	}
	else
		return TRUE;
	//MoveWindow(0, 0, 210, 60);
	::MoveWindow(m_pFileSearch->GetHWnd(), 2, 4, 200, 150, TRUE);
	m_pFileSearch->AddSearchFolder("D:\\");
	m_pFileSearch->AddSearchFolder("E:\\En8000");
	m_pFileSearch->AddSearchFolder("F:\\");
	m_pFileSearch->AddSearchFolder("C:\\");
	m_pFileSearch->ToSearch();//准备搜索
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

void CTestFileSearchDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTestFileSearchDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
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

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CTestFileSearchDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestFileSearchDlg::OnDestroy()
{
	CDialog::OnDestroy();
}

void CTestFileSearchDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialog::OnOK();
}

LRESULT CTestFileSearchDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL CTestFileSearchDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_SPACE || pMsg->wParam == VK_RETURN)
			return 0;
		break;
	default:
		break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CTestFileSearchDlg::OnBnClickedbtnsubmit()
{
	char buf[100];
	GetDlgItem(IDC_EdtSub)->GetWindowText(buf, 100);
	string str = buf;
	m_pFileSearch->InputStr(str);
}

void CTestFileSearchDlg::OnBnClickedbtnselect()
{
	char buf[100];
	GetDlgItem(IDC_EDIT2)->GetWindowText(buf, 100);
	m_pFileSearch->Select(atoi(buf));
}

void CTestFileSearchDlg::OnBnClickedbtngetpath()
{
	GetDlgItem(IDC_EDIT3)->SetWindowText(m_pFileSearch->GetSelectedFilePath().c_str());
}
