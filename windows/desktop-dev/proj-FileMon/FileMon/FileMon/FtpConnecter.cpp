#include "StdAfx.h"
#include "FtpConnecter.h"
#include "PostMsg2Php.h"

std::vector<CString> FtpConnecter::s_FileFailedUpload;
CString FtpConnecter::s_strLocalRootPath = _T("D:\\home\\xampp\\htdocs\\disk\\filestores");
CString FtpConnecter::s_strRemoteRootPath = _T("/xietong/filestores");
BOOL FtpConnecter::s_bRetryUpload = TRUE;		//默认要重试上传一次上次上传失败的文件
BOOL FtpConnecter::s_bHasUploadFailedFile = FALSE;	
BOOL FtpConnecter::s_bStopTask = FALSE;
BOOL FtpConnecter::s_bThreadRunning = FALSE;

FtpConnecter::FtpConnecter(void)
{
	initTimeInfo();
	m_hWorkThread = NULL;
}

FtpConnecter::~FtpConnecter(void)
{
	if (m_FtpInfo._hWaitableTimer != NULL)
	{
		CancelWaitableTimer(m_FtpInfo._hWaitableTimer);
		CloseHandle(m_FtpInfo._hWaitableTimer);
	}
	if (m_hWorkThread)
		CloseHandle(m_hWorkThread);
}
//启动上传文件的线程
DWORD WINAPI FtpConnecter::UploadeFile(LPVOID param)
{
	s_bThreadRunning = TRUE;//记录上传线程已启动
	FtpInfo* pFtpInfo = (FtpInfo*)param;
	//连接FTP服务器，并开始推送文件
	static int count = 0;
	CString strCount;
	try
	{
		while (1)
		{
			if (s_bStopTask)//如果要停止任务那么就跳出
				break;
			pushTrackInfo(L"等待下一次上传任务");
			if (WaitForSingleObject(pFtpInfo->_hWaitableTimer, INFINITE) != WAIT_OBJECT_0)
				break;
			//更新本次上传的本地和远程文件夹的信息
			PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), WM_UpdateRemoteFolderPath, 0, 0);
			PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), WM_UpdateLocalFolderPath, 0, 0);
			if (count == 0)
			{
				count++;
				pushTrackInfo(L"首次启动FTP上传线程，不做上传任务");
				continue;
			}
			pushTrackInfo(L"开始上传任务");
			strCount.Format(_T("开始第%d次上传任务"), ++count);
			pushTrackInfo(strCount);
			if (s_bStopTask)//如果要停止任务那么就跳出
				break;
			CInternetSession InternetSession(_T("MR"), INTERNET_OPEN_TYPE_PRECONFIG);
			CFtpConnection* pFtpconnection = InternetSession.GetFtpConnection(
				pFtpInfo->_strFtpServerIp, 
				pFtpInfo->_strUserName, 
				pFtpInfo->_strUserPw, 
				pFtpInfo->_nFtpServerPort, 
				TRUE);

			CString strRemotePath = getRemoteFolderPath();
			CString strLocalPath = getLocalFolderPath();
			pushTrackInfo(_T("上传本地文件夹 ") + strLocalPath);
			pushTrackInfo(_T("到FTP服务器上 ") + strRemotePath);
			std::vector<CString> files;

			//开始上传文件
			//设置ftp服务器上的路径
			CString strCurDir;
			if (strRemotePath.GetAt(strRemotePath.GetLength()-1) != _T('/'))
				strRemotePath += _T('/');
			int pos = strRemotePath.Find(_T('/'), 0);
			
			while (pos != -1)
			{
				strCurDir = strRemotePath.Left(pos);
				strRemotePath = strRemotePath.Right(strRemotePath.GetLength() - pos - 1);
				if (!strCurDir.IsEmpty())
				{
					CFtpFileFind ftpff(pFtpconnection);
					if (!ftpff.FindFile(strCurDir))//如果找不到这个目录，那么就创建之
						pFtpconnection->CreateDirectory(strCurDir);
					pFtpconnection->SetCurrentDirectory(strCurDir);
				}
				pos = strRemotePath.Find(_T('/'), 0);
			}
			
			//获得本地全部要上传的文件
			if (!getAllFilesUpload(files))
				s_bThreadRunning = FALSE;

			PostMsg2Php post2php;
			BOOL bPut;
			CString strFilePath;
			for (std::vector<CString>::iterator iter = files.begin(); iter != files.end(); ++iter)
			{
				strFilePath = strLocalPath + _T("\\") + *iter;
				pushTrackInfo(_T("开始上传 ") + strFilePath);
				bPut = pFtpconnection->PutFile(strFilePath, *iter);
				if (!bPut)//如果上传不成功那么就记录下来
				{
					pushTrackInfo(*iter + _T(" 上传失败"));
					s_FileFailedUpload.push_back(*iter);
					s_bHasUploadFailedFile = TRUE;
				}
				else
				{
					pushTrackInfo(*iter + _T(" 上传成功"));
					post2php.PostResult(strFilePath, PostMsg2Php::Upload, PostMsg2Php::Suc);
				}
			}
			if (s_bRetryUpload && s_bHasUploadFailedFile)//如果要重试上传并且也有文件上传失败
			{
				s_bHasUploadFailedFile = FALSE;
				for (std::vector<CString>::iterator iter = s_FileFailedUpload.begin(); iter != s_FileFailedUpload.end(); ++iter)
				{
					static CString str;
					str = _T("尝试再次上传文件 ") + strLocalPath + _T("\\") + *iter;
					pushTrackInfo(str);
					strFilePath = strLocalPath + _T("\\") + *iter;
					bPut = pFtpconnection->PutFile(strFilePath, *iter);
					if (!bPut)
					{
						pushTrackInfo(L"尝试失败 : " + strLocalPath + _T("\\") + *iter);
						post2php.PostResult(strFilePath, PostMsg2Php::Upload, PostMsg2Php::Failed);
						s_bHasUploadFailedFile = TRUE;
					}
					else 
					{
						pushTrackInfo(L"尝试成功 : " + strLocalPath + _T("\\") + *iter);
					}
				}
			}
			if (pFtpconnection)
			{
				pFtpconnection->Close();
				delete pFtpconnection;
			}
			strCount.Format(_T("第%d次上传任务完成,总共%d个, 成功%d个，失败%d个"), count, files.size(), files.size()-s_FileFailedUpload.size(), s_FileFailedUpload.size());
			pushTrackInfo(strCount);
			s_FileFailedUpload.clear();
		}
	}
	catch (CInternetException* pEx)
	{
		TCHAR szErr[1024];
		pEx->GetErrorMessage(szErr, 1024);
		pushTrackInfo(_T("FTP连接失败"));
		pushTrackInfo(szErr);
		pEx->Delete();
		s_bThreadRunning = FALSE;
		return -1;
	}
	s_bThreadRunning = FALSE;
	return 0;
}
//设置FTP信息，如果重新设置了信息，那么该FTP信息将在下一次启动上传任务的时候起作用
void FtpConnecter::setFtpinfo(CString& _strFtpServerIp, CString& _strUserName, CString& _strUserPw, int _nFtpServerPort)
{
	m_FtpInfo._nFtpServerPort = _nFtpServerPort;
	m_FtpInfo._strFtpServerIp = _strFtpServerIp;
	m_FtpInfo._strUserName = _strUserName;
	m_FtpInfo._strUserPw = _strUserPw;
}
//设置开始上传的时间
void FtpConnecter::setBeginTime(SYSTEMTIME& beginTimeSt)
{
	FILETIME ft;
	SystemTimeToFileTime(&beginTimeSt, &ft);
	LocalFileTimeToFileTime(&ft, &m_BeginTimeFT);
	m_liBeginTime.LowPart = m_BeginTimeFT.dwLowDateTime;
	m_liBeginTime.HighPart = m_BeginTimeFT.dwHighDateTime;
}
//设置上传文件任务执行的时间间隔
void FtpConnecter::setUploadTimePeriod(unsigned long lPeroid)
{
	m_lTimePeriod = lPeroid;
	const long nTimeUnitPerSecond = 1000;//the unit is 1ms，这里是1s
	m_lTimePeriod *= nTimeUnitPerSecond;
}
//设置好相关参数(上传文件的FTP信息，上传文件的时间设置)之后，开始执行设置的定时任务
BOOL FtpConnecter::startUploadTask()
{
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = FALSE;
	sa.lpSecurityDescriptor = NULL;
	DWORD id;
	//设置FTP信息
	m_FtpInfo._hWaitableTimer = CreateWaitableTimer(&sa, FALSE, NULL);
	if (m_FtpInfo._hWaitableTimer == NULL)
		return FALSE;
	m_FtpInfo._strFolderPath = getRemoteFolderPath();

	m_hWorkThread = CreateThread(&sa, 0, UploadeFile, (LPVOID)&m_FtpInfo, 0, &id);
	if (m_hWorkThread == NULL)
	{
		CString strErr = GlobalFunc::formatErrorCode(GetLastError());
		strErr = _T("启动上传任务失败，上传文件线程未启动") + strErr;
		pushTrackInfo(strErr);
		return FALSE;
	}
	SetWaitableTimer(m_FtpInfo._hWaitableTimer, 
		&m_liBeginTime,
		m_lTimePeriod,
		NULL, NULL, TRUE);//支持唤醒计算机<TRUE>
	//CloseHandle(m_FtpInfo._hWaitableTimer);//在本对象析构的时候才关闭这个句柄
	return TRUE;
}
//测试设置的FTP信息是否可以连的上FTP服务器，连接的上就返回true，否则返回false
BOOL FtpConnecter::tryConnectFtp()
{
	//连接FTP服务器，并开始推送文件
	CInternetSession InternetSession(_T("MR"), INTERNET_OPEN_TYPE_PRECONFIG);
	try
	{
		CFtpConnection* pFtpconnection = InternetSession.GetFtpConnection(
			m_FtpInfo._strFtpServerIp, 
			m_FtpInfo._strUserName, 
			m_FtpInfo._strUserPw, 
			m_FtpInfo._nFtpServerPort, 
			TRUE);//设置主动模式<TRUE>
	}
	catch (CInternetException* pEx)
	{
		TCHAR szErr[1024];
		pEx->GetErrorMessage(szErr, 1024);
		pushTrackInfo(_T("FTP连接失败"));
		pushTrackInfo(szErr);
		pEx->Delete();
		return FALSE;
	}
	return TRUE;
}
void FtpConnecter::initTimeInfo()
{
	//这里设置的开始时间是当地时间1985.5.26 02:00:00:00
	SYSTEMTIME st;
	st.wYear = 1985;
	st.wMonth = 5;
	st.wDayOfWeek = 0;
	st.wDay = 26;
	st.wHour = 17;
	st.wMinute = 45;
	st.wSecond = 0;
	st.wMilliseconds = 0;
	setBeginTime(st);
	long lPeroid = TASKPERIOD;//设置一天的时间，单位是1s
	setUploadTimePeriod(lPeroid);
}
//写入跟踪信息，这个函数会将m_bUpdate设置为true
void FtpConnecter::pushTrackInfo(LPCTSTR info)
{
	static CString str;
	str = _T("FtpConnecter: ");
	str.Append(info);
	ProcTracker::getTracker()->pushTrackInfo(str);
}
void FtpConnecter::stopUploadTask()
{
	s_bStopTask = TRUE;
}
//得到本地文件的路径
CString FtpConnecter::getLocalFolderPath()
{
	CString strPath, strYear, strMonth, strDay, strHour, strMinutes;
	getPreviousDate(strYear, strMonth, strDay, strHour, strMinutes);
	strPath = s_strLocalRootPath;
 	strPath.Append(_T("\\"));
 	strPath.Append(strYear);
 	strPath.Append(_T("\\"));
 	strPath.Append(strMonth);
 	strPath.Append(_T("\\"));
 	strPath.Append(strDay);
	return strPath;
}
//得到FTP服务器上的文件路径
CString FtpConnecter::getRemoteFolderPath()
{
	CString strPath, strYear, strMonth, strDay, strHour, strMinutes;
	getPreviousDate(strYear, strMonth, strDay, strHour, strMinutes);
	strPath = s_strRemoteRootPath;
	strPath.Append(_T("/"));
	strPath.Append(strYear);
	strPath.Append(_T("/"));
	strPath.Append(strMonth);
 	strPath.Append(_T("/"));
 	strPath.Append(strDay);
	return strPath;
}
//得到本地需要上传的全部文件
BOOL FtpConnecter::getAllFilesUpload(std::vector<CString>& files)
{
	//遍历本地要上传的文件
	CFileFind ff;
	CString str = getLocalFolderPath() + _T("\\*.*");
	BOOL res = ff.FindFile(str);
	if (!res)
		return FALSE;
	pushTrackInfo(L"开始搜索待上传文件");
	while (res)
	{
		res = ff.FindNextFile();
		CString str = ff.GetFilePath();
		if (!ff.IsDirectory() && !ff.IsDots())
		{
			files.push_back(ff.GetFileName());
			pushTrackInfo(L"待上传文件:" + ff.GetFileName());
		}
	}
	pushTrackInfo(L"待上传文件搜索完毕");
	ff.Close();
	return TRUE;
}
void FtpConnecter::getPreviousDate(CString& strYear, CString& strMonth, CString& strDay, CString& strHour, CString& strMinutes)
{
	SYSTEMTIME st;
	FILETIME ft;
	GetLocalTime(&st);//得到的就应该是当地时间，不应该用GetSystemTime();
	st.wDayOfWeek = 0;
	st.wHour = 0;
	st.wMinute = 0;
	st.wSecond = 0;
	st.wMilliseconds = 0;
	SystemTimeToFileTime(&st, &ft);
	ft.dwLowDateTime -= 10;//将时间设置为两天交接点，再减去一个值，这样来得到上一天的时间
	FileTimeToSystemTime(&ft, &st);
	st.wDayOfWeek = 0;
	strYear.Format(_T("%4d"), st.wYear);
	strMonth.Format(_T("%02d"), st.wMonth);
	strDay.Format(_T("%02d"), st.wDay);
	strHour.Format(_T("%02d"), st.wHour);
	strMinutes.Format(_T("%02d"), st.wMinute);
}
void FtpConnecter::getCurrentDate(CString& strYear, CString& strMonth, CString& strDay, CString& strHour, CString& strMinutes)
{
	SYSTEMTIME st;
	FILETIME ft, utcft;
	GetLocalTime(&st);
	SystemTimeToFileTime(&st, &ft);
	ft.dwLowDateTime -= 10;
	LocalFileTimeToFileTime(&ft, &utcft);
	FileTimeToSystemTime(&utcft, &st);
	strYear.Format(_T("%4d"), st.wYear);
	strMonth.Format(_T("%02d"), st.wMonth);
	strDay.Format(_T("%02d"), st.wDay);
	strHour.Format(_T("%02d"), st.wHour);
	strMinutes.Format(_T("%02d"), st.wMinute);
}
void FtpConnecter::setLocalRootPath(CString strLocalRootPath)
{
	strLocalRootPath.Trim();
	s_strLocalRootPath = strLocalRootPath;
}
void FtpConnecter::setRemoteRootPath(CString strRemoteRootPath)
{
	strRemoteRootPath.Trim();
	s_strRemoteRootPath = strRemoteRootPath;
}
CString FtpConnecter::getCurLocalFolderPath()
{
	return getLocalFolderPath();
}
//得到当前FTP服务器上的目标路径
CString FtpConnecter::getCurRemoteFolderPath()
{
	return getRemoteFolderPath();
}
BOOL FtpConnecter::getThreadRunning()
{
	return s_bThreadRunning;
}
CString FtpConnecter::getUploadBeginTime()
{
	SYSTEMTIME st;
	FILETIME lft;
	FileTimeToLocalFileTime(&m_BeginTimeFT, &lft);
	FileTimeToSystemTime(&lft, &st);
	CString str;
	if (st.wHour <= 12)
		str.Format(_T("上午%02d:%02d:%02d"), st.wHour, st.wMinute, st.wSecond);
	else
		str.Format(_T("下午%02d:%02d:%02d"), st.wHour, st.wMinute, st.wSecond);
	return str;
}