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
	//��������Ϣд���ļ�
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

//ѯ���Ƿ����µ���Ϣ
bool ProcTracker::update()
{
	return m_bUpdate;
}
//ȡ����ǰ�ĸ�����Ϣ����������Ὣm_bUpdate����Ϊfalse
void ProcTracker::getTrackerInfo(std::list<CString>& trackInfo)
{
	EnterCriticalSection(&m_TrackInfoListCS);
	trackInfo.assign(m_TrackInfoList.begin(), m_TrackInfoList.end());
	m_bUpdate = false;
	LeaveCriticalSection(&m_TrackInfoListCS);
}
//д�������Ϣ����������Ὣm_bUpdate����Ϊtrue
void ProcTracker::pushTrackInfo(CString& strTrack)
{
	EnterCriticalSection(&m_TrackInfoListCS);
	while (m_TrackInfoList.size() >= m_InfoUpperLimit)
		m_TrackInfoList.pop_front();
	static int count = 0;
	count++;
	if (count > MAX_TRACKINFO)
	{
		count = 0;//���¼���
		TrackInfo2File();
	}
	static SYSTEMTIME st;
	GetLocalTime(&st);
	static CString str;
	//���ټ�¼��ͷ��
	str.Format(_T("%04d-%02d-%02d %02d:%02d:%02d:%05d "), st.wYear, st.wMonth, /*st.wDayOfWeek, */st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	m_TrackInfoList.push_back(str + strTrack);
	m_bUpdate = true;
	LeaveCriticalSection(&m_TrackInfoListCS);
}
//����ά������Ϣ������
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
	//�����ɶ����е�����������ָ������������ô�ͽ�����д�뵽�����ļ���
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = FALSE;
	sa.lpSecurityDescriptor = NULL;
	CString strPath;
	//��־Ĭ��ʹ��Ŀ¼ΪLog
	strPath = GlobalFunc::getCurrentPath() + _T("Log");
	if (!GlobalFunc::DirectoryExist(strPath))
		GlobalFunc::CreateDirectory(strPath);
	//�趨��־������Ϊʱ�䣬���Ϊһ��Сʱһ���ļ�
	SYSTEMTIME st;
	GetLocalTime(&st);
	strPath.Format(_T("%04d%02d%02d%02d%02d"), st.wYear, st.wMonth, st.wDay, st.wHour, 0/*st.wMinute*/);
	strPath = GlobalFunc::getCurrentPath() + _T("Log\\") + strPath + _T(".txt");
	HANDLE hFile = CreateFile(strPath, FILE_APPEND_DATA, 0, &sa, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		CString str = _T("��⵽������Ϣ������������ڽ�������Ϣ����д�뱾���ļ�ʱʧ�ܣ�");
		pushTrackInfo(str);
		str = GlobalFunc::formatErrorCode(GetLastError());
		pushTrackInfo(str);
		m_TrackInfoList.clear();//��ȻҪ����ڴ�
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
	m_TrackInfoList.clear();//����ڴ�
}