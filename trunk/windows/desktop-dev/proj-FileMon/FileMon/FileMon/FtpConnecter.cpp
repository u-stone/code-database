#include "StdAfx.h"
#include "FtpConnecter.h"
#include "PostMsg2Php.h"

std::vector<CString> FtpConnecter::s_FileFailedUpload;
CString FtpConnecter::s_strLocalRootPath = _T("D:\\home\\xampp\\htdocs\\disk\\filestores");
CString FtpConnecter::s_strRemoteRootPath = _T("/xietong/filestores");
BOOL FtpConnecter::s_bRetryUpload = TRUE;		//Ĭ��Ҫ�����ϴ�һ���ϴ��ϴ�ʧ�ܵ��ļ�
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
//�����ϴ��ļ����߳�
DWORD WINAPI FtpConnecter::UploadeFile(LPVOID param)
{
	s_bThreadRunning = TRUE;//��¼�ϴ��߳�������
	FtpInfo* pFtpInfo = (FtpInfo*)param;
	//����FTP������������ʼ�����ļ�
	static int count = 0;
	CString strCount;
	try
	{
		while (1)
		{
			if (s_bStopTask)//���Ҫֹͣ������ô������
				break;
			pushTrackInfo(L"�ȴ���һ���ϴ�����");
			if (WaitForSingleObject(pFtpInfo->_hWaitableTimer, INFINITE) != WAIT_OBJECT_0)
				break;
			//���±����ϴ��ı��غ�Զ���ļ��е���Ϣ
			PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), WM_UpdateRemoteFolderPath, 0, 0);
			PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), WM_UpdateLocalFolderPath, 0, 0);
			if (count == 0)
			{
				count++;
				pushTrackInfo(L"�״�����FTP�ϴ��̣߳������ϴ�����");
				continue;
			}
			pushTrackInfo(L"��ʼ�ϴ�����");
			strCount.Format(_T("��ʼ��%d���ϴ�����"), ++count);
			pushTrackInfo(strCount);
			if (s_bStopTask)//���Ҫֹͣ������ô������
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
			pushTrackInfo(_T("�ϴ������ļ��� ") + strLocalPath);
			pushTrackInfo(_T("��FTP�������� ") + strRemotePath);
			std::vector<CString> files;

			//��ʼ�ϴ��ļ�
			//����ftp�������ϵ�·��
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
					if (!ftpff.FindFile(strCurDir))//����Ҳ������Ŀ¼����ô�ʹ���֮
						pFtpconnection->CreateDirectory(strCurDir);
					pFtpconnection->SetCurrentDirectory(strCurDir);
				}
				pos = strRemotePath.Find(_T('/'), 0);
			}
			
			//��ñ���ȫ��Ҫ�ϴ����ļ�
			if (!getAllFilesUpload(files))
				s_bThreadRunning = FALSE;

			PostMsg2Php post2php;
			BOOL bPut;
			CString strFilePath;
			for (std::vector<CString>::iterator iter = files.begin(); iter != files.end(); ++iter)
			{
				strFilePath = strLocalPath + _T("\\") + *iter;
				pushTrackInfo(_T("��ʼ�ϴ� ") + strFilePath);
				bPut = pFtpconnection->PutFile(strFilePath, *iter);
				if (!bPut)//����ϴ����ɹ���ô�ͼ�¼����
				{
					pushTrackInfo(*iter + _T(" �ϴ�ʧ��"));
					s_FileFailedUpload.push_back(*iter);
					s_bHasUploadFailedFile = TRUE;
				}
				else
				{
					pushTrackInfo(*iter + _T(" �ϴ��ɹ�"));
					post2php.PostResult(strFilePath, PostMsg2Php::Upload, PostMsg2Php::Suc);
				}
			}
			if (s_bRetryUpload && s_bHasUploadFailedFile)//���Ҫ�����ϴ�����Ҳ���ļ��ϴ�ʧ��
			{
				s_bHasUploadFailedFile = FALSE;
				for (std::vector<CString>::iterator iter = s_FileFailedUpload.begin(); iter != s_FileFailedUpload.end(); ++iter)
				{
					static CString str;
					str = _T("�����ٴ��ϴ��ļ� ") + strLocalPath + _T("\\") + *iter;
					pushTrackInfo(str);
					strFilePath = strLocalPath + _T("\\") + *iter;
					bPut = pFtpconnection->PutFile(strFilePath, *iter);
					if (!bPut)
					{
						pushTrackInfo(L"����ʧ�� : " + strLocalPath + _T("\\") + *iter);
						post2php.PostResult(strFilePath, PostMsg2Php::Upload, PostMsg2Php::Failed);
						s_bHasUploadFailedFile = TRUE;
					}
					else 
					{
						pushTrackInfo(L"���Գɹ� : " + strLocalPath + _T("\\") + *iter);
					}
				}
			}
			if (pFtpconnection)
			{
				pFtpconnection->Close();
				delete pFtpconnection;
			}
			strCount.Format(_T("��%d���ϴ��������,�ܹ�%d��, �ɹ�%d����ʧ��%d��"), count, files.size(), files.size()-s_FileFailedUpload.size(), s_FileFailedUpload.size());
			pushTrackInfo(strCount);
			s_FileFailedUpload.clear();
		}
	}
	catch (CInternetException* pEx)
	{
		TCHAR szErr[1024];
		pEx->GetErrorMessage(szErr, 1024);
		pushTrackInfo(_T("FTP����ʧ��"));
		pushTrackInfo(szErr);
		pEx->Delete();
		s_bThreadRunning = FALSE;
		return -1;
	}
	s_bThreadRunning = FALSE;
	return 0;
}
//����FTP��Ϣ�����������������Ϣ����ô��FTP��Ϣ������һ�������ϴ������ʱ��������
void FtpConnecter::setFtpinfo(CString& _strFtpServerIp, CString& _strUserName, CString& _strUserPw, int _nFtpServerPort)
{
	m_FtpInfo._nFtpServerPort = _nFtpServerPort;
	m_FtpInfo._strFtpServerIp = _strFtpServerIp;
	m_FtpInfo._strUserName = _strUserName;
	m_FtpInfo._strUserPw = _strUserPw;
}
//���ÿ�ʼ�ϴ���ʱ��
void FtpConnecter::setBeginTime(SYSTEMTIME& beginTimeSt)
{
	FILETIME ft;
	SystemTimeToFileTime(&beginTimeSt, &ft);
	LocalFileTimeToFileTime(&ft, &m_BeginTimeFT);
	m_liBeginTime.LowPart = m_BeginTimeFT.dwLowDateTime;
	m_liBeginTime.HighPart = m_BeginTimeFT.dwHighDateTime;
}
//�����ϴ��ļ�����ִ�е�ʱ����
void FtpConnecter::setUploadTimePeriod(unsigned long lPeroid)
{
	m_lTimePeriod = lPeroid;
	const long nTimeUnitPerSecond = 1000;//the unit is 1ms��������1s
	m_lTimePeriod *= nTimeUnitPerSecond;
}
//���ú���ز���(�ϴ��ļ���FTP��Ϣ���ϴ��ļ���ʱ������)֮�󣬿�ʼִ�����õĶ�ʱ����
BOOL FtpConnecter::startUploadTask()
{
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = FALSE;
	sa.lpSecurityDescriptor = NULL;
	DWORD id;
	//����FTP��Ϣ
	m_FtpInfo._hWaitableTimer = CreateWaitableTimer(&sa, FALSE, NULL);
	if (m_FtpInfo._hWaitableTimer == NULL)
		return FALSE;
	m_FtpInfo._strFolderPath = getRemoteFolderPath();

	m_hWorkThread = CreateThread(&sa, 0, UploadeFile, (LPVOID)&m_FtpInfo, 0, &id);
	if (m_hWorkThread == NULL)
	{
		CString strErr = GlobalFunc::formatErrorCode(GetLastError());
		strErr = _T("�����ϴ�����ʧ�ܣ��ϴ��ļ��߳�δ����") + strErr;
		pushTrackInfo(strErr);
		return FALSE;
	}
	SetWaitableTimer(m_FtpInfo._hWaitableTimer, 
		&m_liBeginTime,
		m_lTimePeriod,
		NULL, NULL, TRUE);//֧�ֻ��Ѽ����<TRUE>
	//CloseHandle(m_FtpInfo._hWaitableTimer);//�ڱ�����������ʱ��Źر�������
	return TRUE;
}
//�������õ�FTP��Ϣ�Ƿ����������FTP�����������ӵ��Ͼͷ���true�����򷵻�false
BOOL FtpConnecter::tryConnectFtp()
{
	//����FTP������������ʼ�����ļ�
	CInternetSession InternetSession(_T("MR"), INTERNET_OPEN_TYPE_PRECONFIG);
	try
	{
		CFtpConnection* pFtpconnection = InternetSession.GetFtpConnection(
			m_FtpInfo._strFtpServerIp, 
			m_FtpInfo._strUserName, 
			m_FtpInfo._strUserPw, 
			m_FtpInfo._nFtpServerPort, 
			TRUE);//��������ģʽ<TRUE>
	}
	catch (CInternetException* pEx)
	{
		TCHAR szErr[1024];
		pEx->GetErrorMessage(szErr, 1024);
		pushTrackInfo(_T("FTP����ʧ��"));
		pushTrackInfo(szErr);
		pEx->Delete();
		return FALSE;
	}
	return TRUE;
}
void FtpConnecter::initTimeInfo()
{
	//�������õĿ�ʼʱ���ǵ���ʱ��1985.5.26 02:00:00:00
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
	long lPeroid = TASKPERIOD;//����һ���ʱ�䣬��λ��1s
	setUploadTimePeriod(lPeroid);
}
//д�������Ϣ����������Ὣm_bUpdate����Ϊtrue
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
//�õ������ļ���·��
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
//�õ�FTP�������ϵ��ļ�·��
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
//�õ�������Ҫ�ϴ���ȫ���ļ�
BOOL FtpConnecter::getAllFilesUpload(std::vector<CString>& files)
{
	//��������Ҫ�ϴ����ļ�
	CFileFind ff;
	CString str = getLocalFolderPath() + _T("\\*.*");
	BOOL res = ff.FindFile(str);
	if (!res)
		return FALSE;
	pushTrackInfo(L"��ʼ�������ϴ��ļ�");
	while (res)
	{
		res = ff.FindNextFile();
		CString str = ff.GetFilePath();
		if (!ff.IsDirectory() && !ff.IsDots())
		{
			files.push_back(ff.GetFileName());
			pushTrackInfo(L"���ϴ��ļ�:" + ff.GetFileName());
		}
	}
	pushTrackInfo(L"���ϴ��ļ��������");
	ff.Close();
	return TRUE;
}
void FtpConnecter::getPreviousDate(CString& strYear, CString& strMonth, CString& strDay, CString& strHour, CString& strMinutes)
{
	SYSTEMTIME st;
	FILETIME ft;
	GetLocalTime(&st);//�õ��ľ�Ӧ���ǵ���ʱ�䣬��Ӧ����GetSystemTime();
	st.wDayOfWeek = 0;
	st.wHour = 0;
	st.wMinute = 0;
	st.wSecond = 0;
	st.wMilliseconds = 0;
	SystemTimeToFileTime(&st, &ft);
	ft.dwLowDateTime -= 10;//��ʱ������Ϊ���콻�ӵ㣬�ټ�ȥһ��ֵ���������õ���һ���ʱ��
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
//�õ���ǰFTP�������ϵ�Ŀ��·��
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
		str.Format(_T("����%02d:%02d:%02d"), st.wHour, st.wMinute, st.wSecond);
	else
		str.Format(_T("����%02d:%02d:%02d"), st.wHour, st.wMinute, st.wSecond);
	return str;
}