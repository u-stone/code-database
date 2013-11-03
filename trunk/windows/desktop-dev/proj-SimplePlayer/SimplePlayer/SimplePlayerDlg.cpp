
// SimplePlayerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SimplePlayer.h"
#include "SimplePlayerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
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

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSimplePlayerDlg �Ի���




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


// CSimplePlayerDlg ��Ϣ�������

BOOL CSimplePlayerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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
	InitDS();
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CSimplePlayerDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
	//����DS�豸
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
