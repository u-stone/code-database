
// ProcTimer.cpp : 定义应用程序的类行为。
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


// CProcTimerApp 构造

CProcTimerApp::CProcTimerApp()
{
	// TODO: 在此处添加构造代码，
	m_bAutoStart = false;
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CProcTimerApp 对象

CProcTimerApp theApp;


// CProcTimerApp 初始化

BOOL CProcTimerApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	CreateRun();//开机启动

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	//SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CProcTimerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
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
	CString   lpszFile=sPath+"\\ProcTimer.exe";//这里加上你要查找的执行文件名称   
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
			AfxMessageBox(_T("打开了注册表项"));
			if (m_bAutoStart)
			{
				AfxMessageBox(_T("添加了注册表项"));
				RegSetValueEx(RegKey,_T("ProcTimer"),0,REG_SZ,(const BYTE*)(LPCTSTR)fullName,sizeof(wchar_t) * (fullName.GetLength()+1));//这里加上你需要在注册表中注册的内容   
			}
			else
			{
				AfxMessageBox(_T("删除了注册表项"));
				RegDeleteKey(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run\\ProcTimer"));
			}
		}
		else
			AfxMessageBox(_T("没有打开注册表项"));

	}   
	else   
	{   
		::AfxMessageBox(_T("没找到执行程序，自动运行失败"));   
		exit(0);   
	}   
}