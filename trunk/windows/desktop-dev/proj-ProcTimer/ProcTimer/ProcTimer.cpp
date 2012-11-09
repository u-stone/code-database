
// ProcTimer.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "ProcTimer.h"
#include "ProcTimerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CProcTimerApp

BEGIN_MESSAGE_MAP(CProcTimerApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CProcTimerApp ����

CProcTimerApp::CProcTimerApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	m_bAutoStart = false;
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CProcTimerApp ����

CProcTimerApp theApp;


// CProcTimerApp ��ʼ��

BOOL CProcTimerApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	CreateRun();//��������

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	//SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CProcTimerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

void CProcTimerApp::CreateRun()
{
	HKEY   RegKey;   
	CString   sPath;   
	GetModuleFileName(NULL,sPath.GetBufferSetLength(MAX_PATH+1),MAX_PATH);   
	sPath.ReleaseBuffer();   
	int   nPos;   
	nPos=sPath.ReverseFind('\\');   
	sPath=sPath.Left(nPos);   
	CString   lpszFile=sPath+"\\ProcTimer.exe";//���������Ҫ���ҵ�ִ���ļ�����   
	CFileFind   fFind;   
	BOOL   bSuccess;
	bSuccess=fFind.FindFile(lpszFile);   
	fFind.Close();   
	if(bSuccess)   
	{   
		CString   fullName;   
		fullName=lpszFile;   
		RegKey=NULL;   
		if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 
			0, 
			KEY_ALL_ACCESS,
			&RegKey))
		{
			AfxMessageBox(_T("����ע�����"));
			if (m_bAutoStart)
			{
				AfxMessageBox(_T("�����ע�����"));
				RegSetValueEx(RegKey,_T("ProcTimer"),0,REG_SZ,(const BYTE*)(LPCTSTR)fullName,sizeof(wchar_t) * (fullName.GetLength()+1));//�����������Ҫ��ע�����ע�������   
			}
			else
			{
				AfxMessageBox(_T("ɾ����ע�����"));
				RegDeleteKey(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run\\ProcTimer"));
			}
		}
		else
			AfxMessageBox(_T("û�д�ע�����"));

	}   
	else   
	{   
		::AfxMessageBox(_T("û�ҵ�ִ�г����Զ�����ʧ��"));   
		exit(0);   
	}   
}