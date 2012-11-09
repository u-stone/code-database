// TestFileSearchDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TestFileSearch.h"
#include "TestFileSearchDlg.h"
#include ".\testfilesearchdlg.h"

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


// CTestFileSearchDlg �Ի���



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


// CTestFileSearchDlg ��Ϣ�������

BOOL CTestFileSearchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��\������...\���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	HMODULE hModule = LoadLibrary(_T("FileSearch.Dll"));
	if (hModule != NULL)
	{
		typedef IFileSearch* (FileSearchFunc)(CWnd*) ;
		FileSearchFunc* func = (FileSearchFunc*)GetProcAddress(hModule, _T("GetFileSearcher"));
//#define N
//#ifdef	N
		m_pFileSearch = func(this);//��Ϊ�����ڣ�ֻ������Ͽ�
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
	m_pFileSearch->ToSearch();//׼������
	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTestFileSearchDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
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
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
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
	// TODO: �ڴ����ר�ô����/����û���

	CDialog::OnOK();
}

LRESULT CTestFileSearchDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: �ڴ����ר�ô����/����û���

	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL CTestFileSearchDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
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
