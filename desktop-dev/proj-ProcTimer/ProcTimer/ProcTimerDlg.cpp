
// ProcTimerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ProcTimer.h"
#include "ProcTimerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString g_ProcPath;
static volatile bool s_QuitProc = false;
UINT processproc(void*);
HANDLE g_hTimer = INVALID_HANDLE_VALUE;
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


// CProcTimerDlg 对话框




CProcTimerDlg::CProcTimerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProcTimerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_Method = 0;
	m_Period = 0;
	memset(&m_st, 0, sizeof(m_st));
}

void CProcTimerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_DtPicker, m_DtCtrl);
	DDX_Control(pDX, IDC_CmbMethod, m_CmbFrequency);
}

BEGIN_MESSAGE_MAP(CProcTimerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	//ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DtPicker, &CProcTimerDlg::OnDtnDatetimechangeDtpicker)
	ON_BN_CLICKED(IDC_BtnProc, &CProcTimerDlg::OnBnClickedBtnproc)
	ON_BN_CLICKED(IDC_BtnExecute, &CProcTimerDlg::OnBnClickedBtnexecute)
	ON_CBN_SELCHANGE(IDC_CmbMethod, &CProcTimerDlg::OnCbnSelchangeCmbmethod)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_TRARMESSAGE,OnTrayMessage)
	ON_BN_CLICKED(IDOK, &CProcTimerDlg::OnBnClickedOk)
	ON_COMMAND(ID_SHOWMAINUI, OnShowMainUi)
	ON_COMMAND(ID_EXITAPP, OnExitApp)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BtnQuit, &CProcTimerDlg::OnBnClickedBtnquit)
END_MESSAGE_MAP()


// CProcTimerDlg 消息处理程序

BOOL CProcTimerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_menu.LoadMenu(IDR_RBMENU);

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

	//初始化控件
	SYSTEMTIME st;
	
	//m_DtCtrl.GetTime(&st);
	//GetSystemTime(&st);
	GetLocalTime(&st);
	//FILETIME ft, ftUTC;
	//SystemTimeToFileTime(&st, &ft);
	//LocalFileTimeToFileTime(&ft, &ftUTC);
	//FileTimeToSystemTime(&ftUTC, &st);
	memcpy(&m_st, &st, sizeof(st));
	CString str;
	int ids[] = {
		IDC_EdtYear, IDC_EdtMonth, IDC_Edtday, IDC_EdtHour, IDC_Edtminute, IDC_Edtsecond
	};
	WORD times[] = {st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond};
	for (int i = 0; i < sizeof(ids)/sizeof(ids[0]); ++i)
	{
		str.Format(_T("%02d"), times[i]);
		GetDlgItem(ids[i])->SetWindowText(str);
	}
	
	CString strAry[] = {
		_T("每秒"),
		_T("每分"),
		_T("每小时"),
		_T("每天"),
		_T("每月"),
		_T("每年")
	};
	for (int i = 0; i < sizeof(strAry)/sizeof(strAry[0]); ++i)
		m_CmbFrequency.AddString(strAry[i]);
	readInitData();
	//m_Method = 2;
	m_CmbFrequency.SetCurSel(m_Method);
	GetDlgItem(IDC_EdtProc)->SetWindowText(g_ProcPath);


	m_traydata.cbSize = sizeof(NOTIFYICONDATA);
	m_traydata.hIcon = AfxGetApp()->LoadIcon(IDI_TRAYICON);
	m_traydata.hWnd = m_hWnd;
	wchar_t  *m_str = _T("PT主界面");
	wcscpy_s(m_traydata.szTip, 64, m_str);
	m_traydata.uCallbackMessage = WM_TRARMESSAGE;
	m_traydata.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP;

	return TRUE;
}

LRESULT CProcTimerDlg::OnTrayMessage(WPARAM wParam, LPARAM lParam)
{
	if (lParam == WM_LBUTTONDOWN)
	{
		ShowWindow(SW_RESTORE);
	}
	else if (lParam == WM_RBUTTONDOWN)
	{
		CPoint m_point;
		::GetCursorPos(&m_point);
		CMenu* m_submenu = m_menu.GetSubMenu(0);
		m_submenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, m_point.x,m_point.y,AfxGetApp()->m_pMainWnd,TPM_LEFTALIGN);
	}
	return 0;
}

void CProcTimerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if ((nID & 0xFFF0) == SC_MINIMIZE )
	{

		ShowWindow(SW_HIDE);
		Shell_NotifyIcon(NIM_ADD,&m_traydata);

	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CProcTimerDlg::OnPaint()
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
HCURSOR CProcTimerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CProcTimerDlg::OnBnClickedBtnproc()
{
	CFileDialog dlg(TRUE, _T(".exe"), NULL, 4|2, _T("可执行文件(*.exe)|*.exe|全部文件(*.*)|*.*|"));
	if (dlg.DoModal() != IDOK)
	{
		return ;
	}
	g_ProcPath = dlg.GetPathName();
	GetDlgItem(IDC_EdtProc)->SetWindowText(g_ProcPath);
}

void CProcTimerDlg::OnBnClickedBtnexecute()
{
	if (g_ProcPath.IsEmpty())
	{
		AfxMessageBox(_T("先选择要执行的程序!"));
		return ;
	}
	if (g_hTimer == INVALID_HANDLE_VALUE)
		g_hTimer = CreateWaitableTimer(NULL, FALSE, _T("LGY_TIMER"));
	else
	{
		AfxMessageBox(_T("程序运行遇到错误，请重试"));
		return ;
	}
	AfxBeginThread(processproc, NULL);
	GetDlgItem(IDC_BtnExecute)->EnableWindow(FALSE);

	GetUserSystemTime();
	switch (m_Method)
	{
	case SEC:
		AfxMessageBox(_T("你想把机器卡死吗？"));
		m_Period = TSEC;
		m_Period = TMINUTE;
		break;
	case MIN:
		m_Period = TMINUTE;
		break;
	case HOUR:
		m_Period = THOUR;
		break;
	case DAY:
		m_Period = TDAY;
		break;
	case MON:
		m_Period = TMON;
		break;
	case YEAR:
		m_Period = TYEAR;
		break;
	default:
		break;
	}
	FILETIME ft, ftUTC;
	SystemTimeToFileTime(&m_st, &ft);
	LocalFileTimeToFileTime(&ft, &ftUTC);
	LARGE_INTEGER li;
	li.HighPart = ftUTC.dwHighDateTime;
	li.LowPart = ftUTC.dwLowDateTime;
	SetWaitableTimer(g_hTimer, &li, m_Period, NULL, NULL, TRUE);
	if (GetLastError() == ERROR_NOT_SUPPORTED)
	{
		AfxMessageBox(_T("不支持系统唤醒!"));
	}
	//写入配置文件当前的程序设置
	writeInitData();
}


UINT processproc(void*)
{
	for (;;)
	{
		if (WaitForSingleObject(g_hTimer, INFINITE) == WAIT_OBJECT_0)
		{
			//wchar_t * psrc = g_ProcPath.GetBuffer();
			//DWORD size = WideCharToMultiByte(CP_ACP, 0, psrc, -1, NULL, 0, '\0', NULL);
			//char *pnew = new char[size + 1];
			//WideCharToMultiByte(CP_ACP, 0, psrc, -1, pnew, size, '\0', NULL);
			if (s_QuitProc)
				break;
			keybd_event(108,0,0,0);
			keybd_event(108,0,KEYEVENTF_KEYUP,0);
			CString strFile, strDirectory;
			strDirectory = g_ProcPath.Left(g_ProcPath.ReverseFind('\\'));
			strFile = g_ProcPath.Right(g_ProcPath.GetLength() - g_ProcPath.ReverseFind('\\')-1);
			ShellExecute(NULL, _T("open"), strFile, NULL, strDirectory, SW_SHOWDEFAULT);
			//WinExec(pnew, SW_SHOW);
			//delete []pnew;
			//g_ProcPath.ReleaseBuffer();
		}
	}
	return 0;
}

void CProcTimerDlg::OnCbnSelchangeCmbmethod()
{
	m_Method = m_CmbFrequency.GetCurSel();
}

void CProcTimerDlg::OnDestroy()
{
	CDialog::OnDestroy();

	PreExit();
	// TODO: 在此处添加消息处理程序代码
}


void CProcTimerDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	ShowWindow(SW_HIDE);
	Shell_NotifyIcon(NIM_ADD,&m_traydata);
}
void CProcTimerDlg::OnShowMainUi()
{
	ShowWindow(SW_RESTORE);
}
void CProcTimerDlg::OnExitApp()
{
	//PreExit();
	CDialog::OnOK();
}

void CProcTimerDlg::OnClose()
{
	//PreExit();
	CDialog::OnClose();
}

void CProcTimerDlg::PreExit()
{
	s_QuitProc = true;
	CloseHandle(g_hTimer);//关闭句柄
	CancelWaitableTimer(g_hTimer);//取消定时器
	Shell_NotifyIcon(NIM_DELETE,&m_traydata);
}
void CProcTimerDlg::GetUserSystemTime()
{
	int ids[] = { IDC_EdtYear, IDC_EdtMonth, IDC_Edtday, IDC_EdtHour, IDC_Edtminute, IDC_Edtsecond };
	WORD times[6] = {0};
	CString str;
	for (int i = 0; i < sizeof(ids)/sizeof(ids[0]); ++i)
	{
		GetDlgItem(ids[i])->GetWindowText(str);
		times[i] = _wtoi(str.GetBuffer());
		str.ReleaseBuffer();
	}
	m_st.wYear = times[0];
	m_st.wMonth = times[1];
	m_st.wDay = times[2];
	m_st.wHour = times[3];
	m_st.wMinute = times[4];
	m_st.wSecond = times[5];
}

void CProcTimerDlg::OnBnClickedBtnquit()
{
	//PreExit();
	CDialog::OnOK();
}

bool CProcTimerDlg::readInitData()
{
	//配置文件中的日期数据不变
	TCHAR path[MAX_PATH] = {0};
	CString strPath = getInitFilePath();
	GetPrivateProfileString(_T("ProcPath"), _T("Path"), _T(""), path, MAX_PATH, strPath);
	g_ProcPath = path;
	m_Method = GetPrivateProfileInt(_T("Frequency"), _T("Freq"), 3, strPath);
	return true;
}
void CProcTimerDlg::writeInitData()
{
	CString strPath = getInitFilePath();
	BOOL res = WritePrivateProfileString(_T("ProcPath"), _T("Path"), g_ProcPath, strPath);
	if (!res)
	{
		AfxMessageBox(_T("Windows 7下不能写设置文件"));
		return ;
	}
	CString str;
	str.Format(_T("%d"), m_Method);
	WritePrivateProfileString(_T("Frequency"), _T("Freq"), str, strPath);
}
CString CProcTimerDlg::getInitFilePath()
{
	TCHAR path[MAX_PATH]={0};
	GetModuleFileName(NULL, path, MAX_PATH);
	CString strPath = path;
	strPath = strPath.Left(strPath.ReverseFind(_T('\\'))) + _T("\\init.ini");
	return strPath;
}