#pragma once


class CWindowsService 
{ 
public: 
	CWindowsService(void); 
public: 
	~CWindowsService(void); 
	enum SEV_STATUS 
	{ 
		SEV_ERROR = 0x00, 
		SEV_NO  = 0x01, 
		SEV_HAVE = 0x02, 
		SEV_RUNING = 0x03, 
		SEV_STOPED = 0x04 
	}; 
public: 
	BOOL AddService(const TCHAR*pSourceName,const TCHAR*pServiceName,const TCHAR*pDisName,const TCHAR*pPara); 
	BOOL RemoveService(const TCHAR*pServiceName); 
	BYTE CheckServiceStatus(const TCHAR*pServiceName); 
	BOOL StartSevice(const TCHAR*pServiceName); 
	BOOL StopSevice(const TCHAR*pServiceName); 
	BOOL GetSevicePath(const TCHAR*pServiceName,CString &strServicePath); 
	BOOL GetCurPath(CString &strCurPath); 
};