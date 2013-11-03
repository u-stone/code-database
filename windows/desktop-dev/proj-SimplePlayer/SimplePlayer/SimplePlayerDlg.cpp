
// SimplePlayerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SimplePlayer.h"
#include "SimplePlayerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
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

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSimplePlayerDlg 对话框




CSimplePlayerDlg::CSimplePlayerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSimplePlayerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	InitMember();
}

void CSimplePlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSimplePlayerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BtnOpenFile, &CSimplePlayerDlg::OnBnClickedBtnopenfile)
	ON_BN_CLICKED(IDC_BtnPlay, &CSimplePlayerDlg::OnBnClickedBtnplay)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CSimplePlayerDlg 消息处理程序

BOOL CSimplePlayerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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
	InitDS();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSimplePlayerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSimplePlayerDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSimplePlayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSimplePlayerDlg::OnBnClickedBtnopenfile()
{
	CFileDialog dlg(TRUE);
	if (dlg.DoModal() == IDOK){
		CString strFilePath = dlg.GetPathName();
		GetDlgItem(IDC_EdtFilePath)->SetWindowText(strFilePath);
	}
}

void CSimplePlayerDlg::OnBnClickedBtnplay()
{
	if (m_pPalyer != NULL)
		return ;
	m_pPalyer = new WavePlayer(m_pIDS);
	CString strFilePath;
	GetDlgItem(IDC_EdtFilePath)->GetWindowText(strFilePath);	
	m_pPalyer->Play(strFilePath);
}

void CSimplePlayerDlg::InitMember()
{
	m_hPlayer = NULL;
	m_pPalyer = NULL;
	m_bInit = FALSE;
	m_pIDS = NULL;
}


BOOL CSimplePlayerDlg::InitDS()
{
	m_bInit = FALSE;
	//创建DS设备
	HRESULT hr = CoInitialize(NULL);
	hr = CoCreateInstance(CLSID_DirectSound, NULL, CLSCTX_INPROC_SERVER, IID_IDirectSound, (void**)&m_pIDS);
	if (FAILED(hr) || m_pIDS == NULL){
		CoUninitialize();
		return m_bInit;
	}
	GUID guid;
	ZeroMemory(&guid, sizeof(GUID));
	hr = m_pIDS->Initialize(&guid);
	if (FAILED(hr)){
		m_pIDS->Release();
		CoUninitialize();
	}
	m_hPlayer = GetSafeHwnd();
	CreatePrimaryDSB();

	m_bInit = TRUE;
	return m_bInit;
}

BOOL CSimplePlayerDlg::UninitDS()
{
	if (m_pIDS != NULL){
		CoUninitialize();
		m_pIDS->Release();
		m_pIDS = NULL;
	}
	if (m_pPalyer != NULL){
		delete m_pPalyer;
		m_pPalyer = NULL;
	}
	return TRUE;
}

BOOL CSimplePlayerDlg::CreatePrimaryDSB()
{
	if (m_pIDS == NULL || m_hPlayer == NULL)
		return FALSE;
	DWORD size;
	if (acmMetrics(NULL, ACM_METRIC_MAX_SIZE_FORMAT, &size) != MMSYSERR_NOERROR)
		return FALSE;
	WAVEFORMATEX *pwf = (WAVEFORMATEX*)::VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE);
	if (pwf == NULL)
		return FALSE;
	pwf->cbSize = 0;
	pwf->wFormatTag = WAVE_FORMAT_PCM;
	pwf->nChannels = 2;
	pwf->nSamplesPerSec = 22050;
	pwf->nAvgBytesPerSec = 22050 * 2 * 2;
	pwf->nBlockAlign = 4;
	pwf->wBitsPerSample = 16;

	HRESULT hr = m_pIDS->SetCooperativeLevel(m_hPlayer, DSSCL_PRIORITY);
	DSBUFFERDESC dsbd;
	if (SUCCEEDED(hr)){
		ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
		dsbd.dwSize = sizeof(dsbd);
		dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;
		hr = m_pIDS->CreateSoundBuffer(&dsbd, &m_pPrimaryBuffer, NULL);
		if (SUCCEEDED(hr)){
			hr = m_pPrimaryBuffer->SetFormat(pwf);
			if (FAILED(hr)){
				m_pPrimaryBuffer->Release();
				m_pPrimaryBuffer = NULL;
				return FALSE;
			}
			hr = m_pPrimaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
			if (FAILED(hr)){
				m_pPrimaryBuffer->Release();
				m_pPrimaryBuffer = NULL;
				return FALSE;
			}
		} else {
			return FALSE;
		}
	}
	return TRUE;
}


void CSimplePlayerDlg::OnClose()
{
	UninitDS();
	CDialogEx::OnClose();
}
