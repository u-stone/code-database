
// FileMonDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FileMon.h"
#include "FileMonDlg.h"
#include "ConvertManager.h"
#include <algorithm>
#include "FtpConnecter.h"
#include "Base64.h"

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


// CFileMonDlg 对话框




CFileMonDlg::CFileMonDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileMonDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFileMonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTPaths, m_PathListCtrl);
}

BEGIN_MESSAGE_MAP(CFileMonDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BtnMonitoring, &CFileMonDlg::OnBnClickedBtnmonitoring)
	ON_BN_CLICKED(IDC_BtnAddPath, &CFileMonDlg::OnBnClickedBtnaddpath)
	ON_BN_CLICKED(IDC_BtnDelPath, &CFileMonDlg::OnBnClickedBtndelpath)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CFileMonDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BtnOpenConvertProc, &CFileMonDlg::OnBnClickedBtnopenconvertproc)
	ON_BN_CLICKED(IDC_BtnExeConvertProc, &CFileMonDlg::OnBnClickedBtnexeconvertproc)
	ON_BN_CLICKED(IDC_BtnTryConnect, &CFileMonDlg::OnBnClickedBtntryconnect)
	ON_BN_CLICKED(IDC_ChkShowPw, &CFileMonDlg::OnBnClickedChkshowpw)
	ON_COMMAND(ID_SHOWMAINUI, OnShowMainUi)
	ON_COMMAND(ID_EXITAPP, OnExitApp)
	ON_MESSAGE(WM_TRARMESSAGE,OnTrayMessage)
	ON_MESSAGE(WM_UpdateRemoteFolderPath,OnUpdateRemoteFolderPath)
	ON_MESSAGE(WM_UpdateLocalFolderPath,OnUpdateLocalFolderPath)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BtnFlushLog, &CFileMonDlg::OnBnClickedBtnflushlog)
	ON_BN_CLICKED(IDC_BtnSwitchConsole, &CFileMonDlg::OnBnClickedBtnswitchconsole)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CFileMonDlg 消息处理程序

BOOL CFileMonDlg::OnInitDialog()
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

	//ShowWindow(SW_MAXIMIZE);
	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	SetTimer(1, 10, NULL);
	SetTimer(2, 5000, NULL);//每5s更新一次线程的状态
	ReadInit();
	GetClientRect(&m_ClientRect);
	m_ClientRect.left = LONG(m_ClientRect.Width() * 0.31);

	m_bShowConsole = true;


	m_traydata.cbSize = sizeof(NOTIFYICONDATA);
	m_traydata.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_traydata.hWnd = m_hWnd;
	wchar_t  *m_str = _T("PT主界面");
	wcscpy_s(m_traydata.szTip, 64, m_str);
	m_traydata.uCallbackMessage = WM_TRARMESSAGE;
	m_traydata.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CFileMonDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CFileMonDlg::OnPaint()
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
		CPaintDC dc(this);
		CFont font;
		font.CreateFont(12, 0, 0, 0, FW_NORMAL, 0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("宋体"));
		CFont* pOldFont = dc.SelectObject(&font);
		dc.SetBkMode(TRANSPARENT);
		int i = 0;
		std::list<CString> trackInfoList;
		ProcTracker::getTracker()->getTrackerInfo(trackInfoList);
		for (std::list<CString>::reverse_iterator iter = trackInfoList.rbegin(); iter != trackInfoList.rend(); ++iter, ++i)
			dc.TextOut(300, 50 + i * 12, *iter);
		dc.SelectObject(pOldFont);
		font.DeleteObject();
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CFileMonDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CFileMonDlg::OnBnClickedBtnmonitoring()
{
	GetDlgItem(IDC_BtnMonitoring)->EnableWindow(FALSE);
	//开始监视选定的路径
	CString str;
	for (int i = 0; i < m_PathListCtrl.GetCount(); ++i)
	{
		m_PathListCtrl.GetText(i, str);
		if (m_Dirs.end() == find(m_Dirs.begin(), m_Dirs.end(), str))
		{
			m_Dirs.push_back(str);
			if (!m_FileMon.addMonitorDir(str))
				ProcTracker::getTracker()->pushTrackInfo(_T("添加路径: ") + str + _T("失败"));
		}
	}
	//开始调度
	ConvertManager::getConvertManagerObj()->startManager();
}

void CFileMonDlg::OnBnClickedBtnaddpath()
{
	WCHAR szDir[MAX_PATH]; 
	BROWSEINFO bi; 
	ITEMIDLIST *pidl;

	bi.hwndOwner = this->m_hWnd; 
	bi.pidlRoot = NULL; 
	bi.pszDisplayName = szDir; 
	bi.lpszTitle = _T("请选择目录");
	bi.ulFlags = BIF_RETURNONLYFSDIRS; 
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage = 0;

	pidl = SHBrowseForFolder(&bi); 
	if(pidl == NULL) 
		return;

	if(SHGetPathFromIDList(pidl, szDir))
		m_PathListCtrl.AddString(szDir);
	GetDlgItem(IDC_BtnMonitoring)->EnableWindow(TRUE);
}

void CFileMonDlg::OnBnClickedBtndelpath()
{
	CString strPath;
	int indexes[255] = {0};
	m_PathListCtrl.GetSelItems(255, indexes);
	for (int i = 0; i < m_PathListCtrl.GetSelCount(); ++i)
	{
		m_PathListCtrl.GetText(indexes[i], strPath);
		std::vector<CString>::iterator  iter = find(m_Dirs.begin(), m_Dirs.end(), strPath);
		if (iter != m_Dirs.end())
		{
			m_PathListCtrl.DeleteString(i);
			m_Dirs.erase(iter);
			m_FileMon.removeMonitorDir(strPath);
		}
	}
}

void CFileMonDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1 && ProcTracker::getTracker()->update())
		InvalidateRect(&m_ClientRect);
	if (nIDEvent == 2)
	{
		//static BOOL bRunning = m_FtpConnecter.getThreadRunning();
		//if (bRunning != m_FtpConnecter.getThreadRunning())
		{
			if (m_FtpConnecter.getThreadRunning())
				GetDlgItem(IDC_STATICFTPTHREADSTATUS)->SetWindowText(_T("FTP上传线程已启动"));
			else
				GetDlgItem(IDC_STATICFTPTHREADSTATUS)->SetWindowText(_T("FTP上传线程未启动"));
		}
		//bRunning = m_FtpConnecter.getThreadRunning();
	}
	CDialog::OnTimer(nIDEvent);
}
void CFileMonDlg::ReadInit()
{
	//这里面存储的应该是全部要监视路径的全路径
	WCHAR mpath[1<<12] = {0};
	DWORD res = GetPrivateProfileString(_T("MonPath"), _T("Path"), _T(""), mpath, 1<<12, GlobalFunc::getCurrentPath(_T("init.ini")));
	if (res != 0)
	{
		CString str = mpath;
		CString strTmp;
		int pos = 0;
		while (1)
		{
			pos = str.Find(_T(';'));
			if (pos == -1)
				break;
			strTmp = str.Left(pos);
			str = str.Right(str.GetLength() - pos - 1);
			m_PathListCtrl.AddString(strTmp);
		}
	}
	WCHAR cpath[MAX_PATH] = {0};
	//读取转换程序的路径
	res = GetPrivateProfileString(_T("ConvertProcPath"), _T("Path"), _T(""), cpath, MAX_PATH, GlobalFunc::getCurrentPath(_T("init.ini")));
	if (res != 0)
	{
		m_ConvertProcPath = cpath;
		GetDlgItem(IDC_EdtConvertProcFullPath)->SetWindowText(m_ConvertProcPath);
	}
	//读取FTP信息
	ZeroMemory(cpath, sizeof(TCHAR)*MAX_PATH);
	res = GetPrivateProfileString(_T("FTP"), _T("IP"), _T(""), cpath, MAX_PATH * sizeof(TCHAR), GlobalFunc::getCurrentPath(_T("init.ini")));
	if (res != 0)
	{
		m_strFtpServerIP = cpath;
		GetDlgItem(IDC_EdtIP)->SetWindowText(m_strFtpServerIP);
	}
	ZeroMemory(cpath, sizeof(TCHAR)*MAX_PATH);
	res = GetPrivateProfileString(_T("FTP"), _T("Port"), _T(""), cpath, MAX_PATH * sizeof(TCHAR), GlobalFunc::getCurrentPath(_T("init.ini")));
	if (res != 0)
	{
		m_strFtpServerPort = cpath;
		GetDlgItem(IDC_EdtFTPPort)->SetWindowText(m_strFtpServerPort);
	}
	ZeroMemory(cpath, sizeof(TCHAR)*MAX_PATH);
	res = GetPrivateProfileString(_T("FTP"), _T("UserName"), _T(""), cpath, MAX_PATH * sizeof(TCHAR), GlobalFunc::getCurrentPath(_T("init.ini")));
	if (res != 0)
	{
		m_strFtpUser = cpath;
		GetDlgItem(IDC_EdtUser)->SetWindowText(m_strFtpUser);
	}
	ZeroMemory(cpath, sizeof(TCHAR)*MAX_PATH);
	res = GetPrivateProfileString(_T("FTP"), _T("Password"), _T(""), cpath, MAX_PATH * sizeof(TCHAR), GlobalFunc::getCurrentPath(_T("init.ini")));
	if (res != 0)
	{
		m_strFtpPassword = cpath;
		GetDlgItem(IDC_EdtPassword)->SetWindowText(m_strFtpPassword);
	}
}
void CFileMonDlg::WriteInit()
{
	CString str;
	if (m_PathListCtrl.GetCount() > 0)
	{
		m_PathListCtrl.GetText(0, str);
		CString strTmp;
		for (int i = 1; i < m_PathListCtrl.GetCount(); ++i)
		{
			m_PathListCtrl.GetText(i, strTmp);
			str.Append(_T(";"));
			str.Append(strTmp);
		}
		str.Append(_T(";"));
		WritePrivateProfileString(_T("MonPath"), _T("Path"), str, GlobalFunc::getCurrentPath(_T("init.ini")));
	}
	GetDlgItem(IDC_EdtConvertProcFullPath)->GetWindowText(m_ConvertProcPath);
	WritePrivateProfileString(_T("ConvertProcPath"), _T("Path"), m_ConvertProcPath, GlobalFunc::getCurrentPath(_T("init.ini")));
	//FTP信息
	GetDlgItem(IDC_EdtIP)->GetWindowText(m_strFtpServerIP);
	GetDlgItem(IDC_EdtFTPPort)->GetWindowText(m_strFtpServerPort);
	GetDlgItem(IDC_EdtUser)->GetWindowText(m_strFtpUser);
	GetDlgItem(IDC_EdtPassword)->GetWindowText(m_strFtpPassword);
	WritePrivateProfileString(_T("FTP"), _T("IP"), m_strFtpServerIP, GlobalFunc::getCurrentPath(_T("init.ini")));
	WritePrivateProfileString(_T("FTP"), _T("Port"), m_strFtpServerPort, GlobalFunc::getCurrentPath(_T("init.ini")));
	WritePrivateProfileString(_T("FTP"), _T("UserName"), m_strFtpUser, GlobalFunc::getCurrentPath(_T("init.ini")));
	WritePrivateProfileString(_T("FTP"), _T("Password"), m_strFtpPassword, GlobalFunc::getCurrentPath(_T("init.ini")));
}
void CFileMonDlg::OnBnClickedOk()
{
	ShowWindow(SW_HIDE);
	Shell_NotifyIcon(NIM_ADD,&m_traydata);
	//WriteInit();
	//OnOK();
}

void CFileMonDlg::OnBnClickedBtnopenconvertproc()
{
	//测试base64加解码的正确性
	//CString cstr = _T("this is another test 刘国元");
	//std::string str = "this is a test 刘国元";
	//Base64 b64;
	//std::string res=b64.Encode((unsigned char*)str.c_str(), str.size());
	//int nout = 0;
	//std::string res2 = b64.Decode(res.c_str(), res.size(), nout);// + " old data: " + res;
	//char buf[1024] = {0};
	//int count = GlobalFunc::UnicodeToANSI(cstr, buf, 1024);
	//CString res3(b64.Encode((unsigned char*)buf, count).c_str());
	//ZeroMemory(buf, 1024);
	//count = GlobalFunc::UnicodeToANSI(res3, buf, 1024);
	//CString res4(b64.Decode(buf, count, nout).c_str());

	CFileDialog dlg(TRUE, _T("exe"), 0, 4|2, _T("可执行文件(.exe)|*.exe||"));
	CString strProcPath;
	if (dlg.DoModal() == IDOK)
	{
		strProcPath = dlg.GetPathName();
		GetDlgItem(IDC_EdtConvertProcFullPath)->SetWindowText(strProcPath);
		WriteInit();
	}
}

void CFileMonDlg::OnBnClickedBtnexeconvertproc()
{
	GetDlgItem(IDC_EdtConvertProcFullPath)->GetWindowText(m_ConvertProcPath);
	CString param = _T("/s /k \"") + m_ConvertProcPath + _T("\"");// + _T(" > ") + ProcTracker::getTracker()->getLogFilePath();
	AfxMessageBox(param);
	ShellExecute(NULL, L"open", _T("cmd.exe"), param, NULL, SW_SHOWNORMAL);
	//ShellExecute(NULL, L"open", m_ConvertProcPath, NULL, NULL, SW_SHOWNORMAL);
	m_bShowConsole = true;
}

void CFileMonDlg::OnBnClickedBtntryconnect()
{
	//
	CString strCurFtpDir;
	int nPort = _wtoi(m_strFtpServerPort.GetBuffer());
	m_strFtpServerPort.ReleaseBuffer();
	GetDlgItem(IDC_EdtIP)->GetWindowText(m_strFtpServerIP);
	GetDlgItem(IDC_EdtFTPPort)->GetWindowText(m_strFtpServerPort);
	GetDlgItem(IDC_EdtUser)->GetWindowText(m_strFtpUser);
	GetDlgItem(IDC_EdtPassword)->GetWindowText(m_strFtpPassword);
	m_FtpConnecter.setFtpinfo(m_strFtpServerIP, m_strFtpUser, m_strFtpPassword, nPort);
	if (m_FtpConnecter.tryConnectFtp())
	{
		AfxMessageBox(_T("连接成功"));
		//GetDlgItem(IDC_EdtRemotePath)->SetWindowText(m_FtpConnecter.getCurRemoteFolderPath());
		//GetDlgItem(IDC_EdtLocalFolderPath)->SetWindowText(m_FtpConnecter.getCurLocalFolderPath());
		GetDlgItem(IDC_EdtuploadFileTime)->SetWindowText(m_FtpConnecter.getUploadBeginTime());
		m_FtpConnecter.startUploadTask();
	}
	else
		AfxMessageBox(_T("连接失败"));
}

void CFileMonDlg::OnBnClickedChkshowpw()
{
	GetDlgItem(IDC_EdtPassword)->GetWindowText(m_strFtpPassword);
	if (((CButton*)GetDlgItem(IDC_ChkShowPw))->GetCheck() == BST_CHECKED)
		((CEdit*)GetDlgItem(IDC_EdtPassword))->SetPasswordChar(0);
	else
		((CEdit*)GetDlgItem(IDC_EdtPassword))->SetPasswordChar(_T('*'));
	
	GetDlgItem(IDC_EdtPassword)->SetWindowText(m_strFtpPassword);
}

BOOL CFileMonDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_F1)
			return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

LRESULT CFileMonDlg::OnTrayMessage(WPARAM wParam, LPARAM lParam)
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
LRESULT CFileMonDlg::OnUpdateRemoteFolderPath(WPARAM wParam, LPARAM lParam)
{
	GetDlgItem(IDC_EdtRemotePath)->SetWindowText(m_FtpConnecter.getCurRemoteFolderPath());
	return 0;
}
LRESULT CFileMonDlg::OnUpdateLocalFolderPath(WPARAM wParam, LPARAM lParam)
{
	GetDlgItem(IDC_EdtLocalFolderPath)->SetWindowText(m_FtpConnecter.getCurLocalFolderPath());
	return 0;
}

void CFileMonDlg::PreExit()
{
	Shell_NotifyIcon(NIM_DELETE,&m_traydata);
}
void CFileMonDlg::OnDestroy()
{	
	CDialog::OnDestroy();

	PreExit();
	WriteInit();
	OnOK();
}
void CFileMonDlg::OnShowMainUi()
{
	ShowWindow(SW_RESTORE);
}
void CFileMonDlg::OnExitApp()
{
	PreExit();
	CDialog::OnOK();
}

void CFileMonDlg::OnBnClickedBtnflushlog()
{
	//强制将日志写入文件
	ProcTracker::getTracker()->ForceInfo2File();
}

void CFileMonDlg::OnBnClickedBtnswitchconsole()
{
	//显示/隐藏控制台程序
	m_bShowConsole = !m_bShowConsole;
	//CString str = L"C:\\Windows\\System32\\cmd.exe - ";
	//str += m_ConvertProcPath;
	//CWnd* pwnd = FindWindow(L"", str);
	//if (pwnd == NULL)
	//	return ;
	//if (m_bShowConsole)
	//	::ShowWindow(pwnd->GetSafeHwnd(), SW_SHOW);
	//else
	//	::ShowWindow(pwnd->GetSafeHwnd(), SW_HIDE);
}

void CFileMonDlg::OnClose()
{
	int res = MessageBox(L"是否要退出程序", L"退出程序", MB_YESNO);
	if (res != 6)
		return ;

	CDialog::OnClose();
}
