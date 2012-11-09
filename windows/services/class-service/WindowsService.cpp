#include "StdAfx.h"
#include "WindowsService.h"
#include <Windows.h>
#include <stdio.h>
#include <WinSvc.h>


CWindowsService::CWindowsService(void)   
{   
}   

CWindowsService::~CWindowsService(void)   
{   
}  
/*****************************************************  
创建服务说明  
如 m_Test.AddService("C://Test//Test.exe","MyServiceName","DisplayName",NULL);  
pPara 为参数有些服务需
*****************************************************/  

BOOL CWindowsService::AddService(const TCHAR*pSourceName,const TCHAR*pServiceName,const TCHAR*pDisName,const TCHAR*pPara)   
{   
	SC_HANDLE hSC = OpenSCManager( NULL, NULL, SC_MANAGER_CREATE_SERVICE );   
	if ( hSC == NULL )   
	{   
		return FALSE;//_ERROR_;    
	}   
	CString strBinnaryPathName;   
	strBinnaryPathName.Format(L"%s%s%s %s",L"\"",pSourceName,L"\"",pPara);   

	SC_HANDLE hSvc = CreateService( hSC,   
		pServiceName,       //服务名称    
		pDisName,          //显示名称    
		SERVICE_ALL_ACCESS,   
		SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,   
		SERVICE_AUTO_START,   
		SERVICE_ERROR_NORMAL,   
		strBinnaryPathName,    
		NULL,   
		NULL,   
		NULL,   
		NULL,   
		NULL );   
	if( hSvc == NULL )   
	{   
		::CloseServiceHandle( hSC );   
		return FALSE;//_ERROR_;    
	}   
	::CloseServiceHandle( hSvc );   
	::CloseServiceHandle( hSC );   
	return TRUE;   
}   


/*  
**************************************************  
删除服务  
**************************************************  
*/   
BOOL CWindowsService::RemoveService(const TCHAR*pServiceName)   
{   
	// UnInstall the XService from Service Control Manager Database    
	SC_HANDLE hSC = ::OpenSCManager( NULL,NULL, SC_MANAGER_ALL_ACCESS);   
	if( hSC == NULL)   
	{   
		return FALSE;   
	}   

	SC_HANDLE hSvc = ::OpenService( hSC, pServiceName, SERVICE_STOP | DELETE/*SERVICE_ALL_ACCESS*/);    
	if( hSvc == NULL)   
	{   
		::CloseServiceHandle( hSC);   
		return FALSE;   
	}

	SERVICE_STATUS status;   
	::ControlService(hSvc, SERVICE_CONTROL_STOP, &status);   

	if ( !::DeleteService(hSvc))
	{
		DWORD _error =GetLastError();//ERROR_ACCESS_DENIED
		return FALSE;
	}  

	::CloseServiceHandle(hSvc); 
	::CloseServiceHandle(hSC);
	return TRUE;  
}   

/*  
**************************************************  
检测状态  
**************************************************  
*/   
BYTE CWindowsService::CheckServiceStatus(const TCHAR*pServiceName)   
{   
	SC_HANDLE hSC = ::OpenSCManager( NULL,NULL, GENERIC_EXECUTE);   
	if( hSC == NULL)   
	{   
		return SEV_ERROR;   
	}   
	// 打开服务。    
	SC_HANDLE hSvc = ::OpenService( hSC, pServiceName, SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);   
	if( hSvc == NULL)   
	{   
		::CloseServiceHandle( hSC);   
		return SEV_NO;   
	}   
	// 获得服务的状态    
	SERVICE_STATUS status;   
	if( ::QueryServiceStatus( hSvc, &status) == FALSE)   
	{   
		// TRACE( "Get Service state error。");    
		::CloseServiceHandle( hSvc);   
		::CloseServiceHandle( hSC);   
		return SEV_ERROR;   
	}   
	if(status.dwCurrentState == SERVICE_RUNNING)   
	{   
		::CloseServiceHandle( hSvc);   
		::CloseServiceHandle( hSC);   
		return SEV_RUNING;   
	}   
	else if(status.dwCurrentState == SERVICE_STOPPED)   
	{   
		::CloseServiceHandle( hSvc);   
		::CloseServiceHandle( hSC);   
		return SEV_STOPED;   
	}   
	::CloseServiceHandle( hSvc);   
	::CloseServiceHandle( hSC);   
	return SEV_ERROR;   
}   
/*  
**************************************************  
启动服务  
**************************************************  
*/   
BOOL CWindowsService::StartSevice(const TCHAR*pServiceName)   
{   
	SC_HANDLE hSC = ::OpenSCManager( NULL,NULL, GENERIC_EXECUTE);   
	if( hSC == NULL)   
	{   
		return FALSE;   
	}   
	// 打开服务。    
	SC_HANDLE hSvc = ::OpenService( hSC, pServiceName, SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);   
	if( hSvc == NULL)   
	{   
		::CloseServiceHandle( hSC);   
		return FALSE;   
	}   
	// 获得服务的状态    
	SERVICE_STATUS status;   
	if( ::QueryServiceStatus( hSvc, &status) == FALSE)   
	{   
		//TRACE( "Get Service state error。");    
		::CloseServiceHandle( hSvc);   
		::CloseServiceHandle( hSC);   
		return FALSE;   
	}

	if( status.dwCurrentState != SERVICE_RUNNING) //如果处于停止状态则启动服务，否则返回      
	{
		// 启动服务
		if( ::StartService( hSvc, NULL, NULL) == FALSE)
		{ 
			//TRACE( "start service error。");
			::CloseServiceHandle( hSvc);   
			::CloseServiceHandle( hSC);   
			return FALSE;   
		}   
		// 等待服务启动    
		while( ::QueryServiceStatus( hSvc, &status) == TRUE)   
		{   
			::Sleep(500);   
			QueryServiceStatus( hSvc, &status);   
			if( status.dwCurrentState == SERVICE_START_PENDING)   
			{   
				//AfxMessageBox( "start success。");    
				::CloseServiceHandle( hSvc);   
				::CloseServiceHandle( hSC);   
				return TRUE;   
			}   
			::Sleep( status.dwWaitHint);   
			QueryServiceStatus( hSvc, &status);   
			if( status.dwCurrentState == SERVICE_RUNNING)   
			{
				//AfxMessageBox( "start success。");    
				::CloseServiceHandle( hSvc);   
				::CloseServiceHandle( hSC);   
				return TRUE;   
			}   
			else   
			{   
				::CloseServiceHandle( hSvc);   
				::CloseServiceHandle( hSC);   
				return FALSE;   
			} 
		}
	}   
	::CloseServiceHandle( hSvc);   
	::CloseServiceHandle( hSC);   
	return TRUE;   
}   
/*  
**************************************************  
停止服务  
**************************************************  
*/   
BOOL CWindowsService::StopSevice(const TCHAR*pServiceName)   
{   
	SC_HANDLE hSC = ::OpenSCManager( NULL,NULL, GENERIC_EXECUTE);   
	if( hSC == NULL)   
	{   
		return FALSE;   
	}   
	// 打开服务。    
	SC_HANDLE hSvc = ::OpenService( hSC, pServiceName,   
		SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);   
	if( hSvc == NULL)   
	{   
		::CloseServiceHandle( hSC);   
		return FALSE;   
	}   
	// 获得服务的状态    
	SERVICE_STATUS status;   
	if( ::QueryServiceStatus( hSvc, &status) == FALSE)   
	{   
		//TRACE( "Get Service state error。");    
		::CloseServiceHandle( hSvc);   
		::CloseServiceHandle( hSC);   
		return FALSE;   
	}   
	//如果处于停止状态则启动服务，否则停止服务。    
	if( status.dwCurrentState == SERVICE_RUNNING)   
	{   
		// 停止服务    
		if( ::ControlService( hSvc, SERVICE_CONTROL_STOP, &status) == FALSE)   
		{   
			//TRACE( "stop service error。");    
			::CloseServiceHandle( hSvc);   
			::CloseServiceHandle( hSC);   
			return FALSE;   
		}   
		// 等待服务停止    
		while( ::QueryServiceStatus( hSvc, &status) == TRUE)   
		{   
			//::Sleep( status.dwWaitHint);    
			::Sleep(1000);   
			QueryServiceStatus( hSvc, &status);   
			if( status.dwCurrentState == SERVICE_STOPPED)   
			{   
				//AfxMessageBox( "stop success。");    
				::CloseServiceHandle( hSvc);   
				::CloseServiceHandle( hSC);   
				return TRUE;   
			}   
			::Sleep(3000);   
			QueryServiceStatus( hSvc, &status);   
			if( status.dwCurrentState == SERVICE_STOPPED)   
			{   
				//AfxMessageBox( "stop success。");    
				::CloseServiceHandle( hSvc);   
				::CloseServiceHandle( hSC);   
				return TRUE;   
			}   
			::Sleep( status.dwWaitHint);   
			QueryServiceStatus( hSvc, &status);   
			if( status.dwCurrentState == SERVICE_STOPPED)   
			{   
				//AfxMessageBox( "stop success。");    
				::CloseServiceHandle( hSvc);   
				::CloseServiceHandle( hSC);   
				return TRUE;   
			}   
			else   
			{   
				::CloseServiceHandle( hSvc);   
				::CloseServiceHandle( hSC);   
				return FALSE;   
			}   
		}   
	}   
	return TRUE;   
}   





BOOL CWindowsService::GetSevicePath(const TCHAR*pServiceName,CString & strServicePath)   
{    
	SC_HANDLE hSC = ::OpenSCManager( NULL,NULL, SC_MANAGER_ALL_ACCESS);   
	if( hSC == NULL)   
	{   
		return FALSE;   
	}   
	// 打开服务。    
	SC_HANDLE hSvc = ::OpenService( hSC, pServiceName,SERVICE_ALL_ACCESS);   
	if( hSvc == NULL)   
	{   
		::CloseServiceHandle( hSC);   
		return FALSE;   
	}   
	LPQUERY_SERVICE_CONFIG  pServiceConfig;   
	DWORD   cbBufSize = 1024*8;//sizeof(SERVICE_DESCRIPTION_STRUCT);    

	DWORD   cbBytesNeeded;   
	CString strPath;   
	pServiceConfig = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LPTR, cbBufSize);      //注意分配足够的空间     
	QueryServiceConfig( hSvc, pServiceConfig  ,cbBufSize, &cbBytesNeeded);    
	strPath = pServiceConfig->lpBinaryPathName;   
	strPath.Replace('"',' ');   
	//int iFind = strPath.Find("////");    
	//strPath = strPath.Left(iFind+1);    
	int iFind = strPath.Find(L"exe");   
	strPath = strPath.Left(iFind+3);   
	strServicePath = strPath;   
	::LocalFree(pServiceConfig);   
	::CloseServiceHandle( hSvc);   
	::CloseServiceHandle( hSC);   

	return TRUE;   
}   


BOOL CWindowsService::GetCurPath(CString &strCurPath)   
{   
	TCHAR szTemp[MAX_PATH] = {0};   
	::GetModuleFileName(NULL, szTemp, sizeof(szTemp)-1);   
	int iLen = (int)wcslen(szTemp);   
	for(int i = iLen - 1; i >= 0; i--)   
	{
		if (szTemp[i] == '//')   
		{   
			break;   
		}   
		else   
		{   
			szTemp[i] = 0;   
		}   
	}   
	strCurPath = szTemp;   
	return TRUE;   
}   