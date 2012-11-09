#include "StdAfx.h"
#include "ProcTracker.h"


ProcTracker* ProcTracker::s_Tracker = NULL;

ProcTracker::ProcTracker(void)
{
	m_bUpdate = false;
	m_bValid = true;
	m_InfoUpperLimit = 100;
	InitializeCriticalSection(&m_TrackInfoListCS);
}

ProcTracker::~ProcTracker(void)
{
	//将跟踪信息写入文件
	TrackInfo2File();
	m_bValid = false;
}

ProcTracker* ProcTracker::getTracker()
{
	if (s_Tracker == NULL)
		s_Tracker = new ProcTracker;
	return s_Tracker;
}

void ProcTracker::deleteTracker()
{
	if (s_Tracker)
	{
		delete s_Tracker;
		s_Tracker = NULL;
	}
}

//询问是否有新的信息
bool ProcTracker::update()
{
	return m_bUpdate;
}
//取出当前的跟踪信息，这个函数会将m_bUpdate设置为false
void ProcTracker::getTrackerInfo(std::list<CString>& trackInfo)
{
	EnterCriticalSection(&m_TrackInfoListCS);
	trackInfo.assign(m_TrackInfoList.begin(), m_TrackInfoList.end());
	m_bUpdate = false;
	LeaveCriticalSection(&m_TrackInfoListCS);
}
//写入跟踪信息，这个函数会将m_bUpdate设置为true
void ProcTracker::pushTrackInfo(CString& strTrack)
{
	EnterCriticalSection(&m_TrackInfoListCS);
	while (m_TrackInfoList.size() >= m_InfoUpperLimit)
		m_TrackInfoList.pop_front();
	static int count = 0;
	count++;
	if (count > MAX_TRACKINFO)
	{
		count = 0;//重新计算
		TrackInfo2File();
	}
	static SYSTEMTIME st;
	GetLocalTime(&st);
	static CString str;
	//跟踪记录的头部
	str.Format(_T("%04d-%02d-%02d %02d:%02d:%02d:%05d "), st.wYear, st.wMonth, /*st.wDayOfWeek, */st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	m_TrackInfoList.push_back(str + strTrack);
	m_bUpdate = true;
	LeaveCriticalSection(&m_TrackInfoListCS);
}
//设置维护的信息的数量
void ProcTracker::setUpperLimit(int nLimit)
{
	if (nLimit > 0)
	{
		m_InfoUpperLimit = nLimit;
		EnterCriticalSection(&m_TrackInfoListCS);
		while (m_TrackInfoList.size() > m_InfoUpperLimit)
			m_TrackInfoList.pop_front();
		LeaveCriticalSection(&m_TrackInfoListCS);
	}
}
bool ProcTracker::isValid()
{
	return m_bValid;
}
void ProcTracker::TrackInfo2File()
{
	if (m_TrackInfoList.size() < MAX_TRACKINFO)
		return ;
	//如果完成队列中的数据量超过指定的数量，那么就将数据写入到本地文件中
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = FALSE;
	sa.lpSecurityDescriptor = NULL;
	CString strPath;
	//日志默认使用目录为Log
	strPath = GlobalFunc::getCurrentPath() + _T("Log");
	if (!GlobalFunc::DirectoryExist(strPath))
		GlobalFunc::CreateDirectory(strPath);
	//设定日志的名称为时间，最大为一个小时一个文件
	SYSTEMTIME st;
	GetLocalTime(&st);
	strPath.Format(_T("%04d%02d%02d%02d%02d"), st.wYear, st.wMonth, st.wDay, st.wHour, 0/*st.wMinute*/);
	strPath = GlobalFunc::getCurrentPath() + _T("Log\\") + strPath + _T(".txt");
	HANDLE hFile = CreateFile(strPath, FILE_APPEND_DATA, 0, &sa, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		CString str = _T("检测到跟踪信息超出额定数量，在将跟踪信息数据写入本地文件时失败！");
		pushTrackInfo(str);
		str = GlobalFunc::formatErrorCode(GetLastError());
		pushTrackInfo(str);
		m_TrackInfoList.clear();//仍然要清空内存
		return ;
	}
	DWORD dwByteWritten = 0;
	CHAR buf[W2FBUFSIZE] = {0};
	CString strAnsi;
	int ansiCount;
	for (std::list<CString>::iterator iter = m_TrackInfoList.begin(); iter != m_TrackInfoList.end(); ++iter)
	{
		ZeroMemory(buf, W2FBUFSIZE);
		strAnsi = *iter;
		ansiCount = GlobalFunc::UnicodeToANSI(strAnsi, buf, W2FBUFSIZE);
		if (iter->GetLength() < BUFSIZE - 2)
		{
			buf[ansiCount] = '\r';
			buf[ansiCount + 1] = '\n';
		}
		WriteFile(hFile, buf, ansiCount + 2, &dwByteWritten, NULL);
	}
	CloseHandle(hFile);
	m_TrackInfoList.clear();//清空内存
}