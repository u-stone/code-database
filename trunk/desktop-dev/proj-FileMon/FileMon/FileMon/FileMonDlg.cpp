
// FileMonDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FileMon.h"
#include "FileMonDlg.h"
#include "ConvertManager.h"
#include <algorithm>
#include "FtpConnecter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CFileMonDlg �Ի���




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
END_MESSAGE_MAP()


// CFileMonDlg ��Ϣ�������

BOOL CFileMonDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	SetTimer(1, 10, NULL);
	SetTimer(2, 5000, NULL);//ÿ5s����һ���̵߳�״̬
	ReadInit();
	GetClientRect(&m_ClientRect);
	m_ClientRect.right = LONG(m_ClientRect.Width() * 0.69);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CFileMonDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CFileMonDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);
		CFont font;
		font.CreateFont(12, 0, 0, 0, FW_NORMAL, 0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("����"));
		CFont* pOldFont = dc.SelectObject(&font);
		dc.SetBkMode(TRANSPARENT);
		int i = 0;
		std::list<CString> trackInfoList;
		ProcTracker::getTracker()->getTrackerInfo(trackInfoList);
		for (std::list<CString>::reverse_iterator iter = trackInfoList.rbegin(); iter != trackInfoList.rend(); ++iter, ++i)
			dc.TextOut(10, 200 + i * 12, *iter);
		dc.SelectObject(pOldFont);
		font.DeleteObject();
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CFileMonDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CFileMonDlg::OnBnClickedBtnmonitoring()
{
	GetDlgItem(IDC_BtnMonitoring)->EnableWindow(FALSE);
	//��ʼ����ѡ����·��
	CString str;
	for (int i = 0; i < m_PathListCtrl.GetCount(); ++i)
	{
		m_PathListCtrl.GetText(i, str);
		if (m_Dirs.end() == find(m_Dirs.begin(), m_Dirs.end(), str))
		{
			m_Dirs.push_back(str);
			if (!m_FileMon.addMonitorDir(str))
				ProcTracker::getTracker()->pushTrackInfo(_T("���·��: ") + str + _T("ʧ��"));
		}
	}
	//��ʼ����
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
	bi.lpszTitle = _T("��ѡ��Ŀ¼");
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
				GetDlgItem(IDC_STATICFTPTHREADSTATUS)->SetWindowText(_T("FTP�ϴ��߳�������"));
			else
				GetDlgItem(IDC_STATICFTPTHREADSTATUS)->SetWindowText(_T("FTP�ϴ��߳�δ����"));
		}
		//bRunning = m_FtpConnecter.getThreadRunning();
	}
	CDialog::OnTimer(nIDEvent);
}
void CFileMonDlg::ReadInit()
{
	//������洢��Ӧ����ȫ��Ҫ����·����ȫ·��
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
	//��ȡת�������·��
	res = GetPrivateProfileString(_T("ConvertProcPath"), _T("Path"), _T(""), cpath, MAX_PATH, GlobalFunc::getCurrentPath(_T("init.ini")));
	if (res != 0)
	{
		m_ConvertProcPath = cpath;
		GetDlgItem(IDC_EdtConvertProcFullPath)->SetWindowText(m_ConvertProcPath);
	}
	//��ȡFTP��Ϣ
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
	//FTP��Ϣ
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
	WriteInit();
	OnOK();
}

void CFileMonDlg::OnBnClickedBtnopenconvertproc()
{
	CFileDialog dlg(TRUE, _T("exe"), 0, 4|2, _T("��ִ���ļ�(.exe)|*.exe||"));
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
	CString param = _T("/s /k \"") + m_ConvertProcPath + _T("\"");
	ShellExecute(NULL, L"open", _T("cmd.exe"), param, NULL, SW_SHOWNORMAL);
	//ShellExecute(NULL, L"open", m_ConvertProcPath, NULL, NULL, SW_SHOWNORMAL);
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
		AfxMessageBox(_T("���ӳɹ�"));
		GetDlgItem(IDC_EdtRemotePath)->SetWindowText(m_FtpConnecter.getCurRemoteFolderPath());
		GetDlgItem(IDC_EdtuploadFileTime)->SetWindowText(m_FtpConnecter.getUploadBeginTime());
		m_FtpConnecter.startUploadTask();
	}
	else
		AfxMessageBox(_T("����ʧ��"));
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
