#include "StdAfx.h"
#include "WinFileMon.h"
#include "FileMonList.h"

WinFileMon::WinFileMon(void)
{
	InitializeCriticalSection(&m_MonInfoCS);
}

WinFileMon::~WinFileMon(void)
{
	EnterCriticalSection(&m_MonInfoCS);
	for (std::set<MonData*>::iterator iter = m_MonitorInfo.begin(); iter != m_MonitorInfo.end(); ++iter)
		delete *iter;
	FileMonList::getFileListObj()->deleteListObj();
	ProcTracker::getTracker()->deleteTracker();
	LeaveCriticalSection(&m_MonInfoCS);
	pushTrackInfo(_T("�˳��ļ�����"));
}

//���Ҫ���ӵ��ļ���
bool WinFileMon::addMonitorDir(CString strDir)
{
	std::vector<CString> vDir;
	int res = checkDir(strDir, vDir);
	if (res == 0)
	{
		pushTrackInfo(_T("���·��: ") + strDir);
		return startMonitorThread(strDir);
	}
	else if (res == 2)
	{
		//��Ҫ��ָ�����ļ�������ļ�ȡ������
		for (std::vector<CString>::iterator iter = vDir.begin(); iter != vDir.end(); ++iter)
			removeMonitorDir(*iter);
		//֮������ʹ���µļ���
		pushTrackInfo(_T("���·��: ") + strDir);
		return startMonitorThread(strDir);
	}
	else//1, 3 ���账��
		;
	return false;
}
//ɾ��Ҫ���ӵ��ļ���
void WinFileMon::removeMonitorDir(CString strDir)
{
	strDir.Trim();
	EnterCriticalSection(&m_MonInfoCS);
	for (std::set<MonData*>::iterator iter = m_MonitorInfo.begin(); iter != m_MonitorInfo.end(); ++iter)
	{
		if ((*iter)->_dir.CompareNoCase(strDir) == 0)
		{
			(*iter)->_bstop = true;
			m_MonitorInfoToDel.insert(*iter);
			m_MonitorInfo.erase(iter);
			pushTrackInfo(_T("ֹͣ����·��: ") + strDir);
			break;
		}
	}
	LeaveCriticalSection(&m_MonInfoCS);
	//std::set<MonData>::iterator iter = find(m_MonitorInfo.begin(), m_MonitorInfo.end(), );
}
//�����µļ����߳�
bool WinFileMon::startMonitorThread(CString strDir)
{
	//���ȳ��Կ��ܷ����һ�����Ѿ���ֹ���̵߳���ز���
	deleteThreadInfo();
	MonData *pData = new MonData();
	pData->_bstop = false;
	pData->_stat = MonData::UNINIT;
	pData->_dir = strDir;
	HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(WinFileMon::monitor), pData, 0, &pData->_thrdID);
	if (SUCCEEDED(hThread))
	{
		CloseHandle(hThread);
		pData->_stat = MonData::NORMAL;
		addMonData(pData);
		pushTrackInfo(_T("��ʼ����·��: ") + pData->_dir);
		return true;
	}
	else
	{
		//û�гɹ���Ҫɾ��new�����ļ������
		delete pData;
		pData = NULL;
		return false;
	}
}

//��Ҫ�����ӵ��ļ����Ƿ��Ѿ��ڼ��ӵ��ļ��л������ļ����У������ǰ��������е�һЩ�ļ���
int WinFileMon::checkDir(CString strDir, std::vector<CString>& dirs)
{
	//��������·���Ƿ���һ���Ѿ����ڵ��ļ���
	//GetFileAttributes()
	if (!PathFileExists(strDir))
		return 3;
	int res = 0;	//���ؽ��
	int len = strDir.GetLength();
	CString strTmp;
	EnterCriticalSection(&m_MonInfoCS);
	//���������ļ���·���Ƿ��������ļ��еĸ�Ŀ¼��Ҳ���Ǽ�������strDir�Ƿ���m_Dirs�е�ĳЩ�ļ���·����ǰ׺
	for (std::set<MonData*>::iterator iter = m_MonitorInfo.begin(); iter != m_MonitorInfo.end(); ++iter)
	{
		if ((*iter)->_dir.GetLength() < len)
			continue;
		strTmp = (*iter)->_dir.Left(len);
		if (strTmp.CompareNoCase(strDir) == 0)
		{
			dirs.push_back((*iter)->_dir);//�����и���Ŀ¼����Ŀ¼·��ȡ����
			res = 2;
		}
	}

	//����������Ƿ񱻰������Ѿ����ڼ��ӵ��ļ����У�Ҳ���Ǽ���Ƿ����Ѿ����ڼ��ӵ��ļ���·����Ϊ�����ļ���·����ǰ׺
	for (std::set<MonData*>::iterator iter = m_MonitorInfo.begin(); iter != m_MonitorInfo.end(); ++iter)
	{
		if ((*iter)->_dir.GetLength() > len)
			continue;
		strTmp = strDir.Left((*iter)->_dir.GetLength());
		if (strTmp.CompareNoCase((*iter)->_dir) == 0)
		{
			res = 1;
			break;
		}
	}
	LeaveCriticalSection(&m_MonInfoCS);
	return res;
}
//���Ӵ���
DWORD WinFileMon::monitor(LPVOID param)
{
	MonData* pData = (MonData*)(param);
	pData->_thrdID = GetCurrentThreadId();
	HANDLE hFile   =   CreateFile( 
		pData->_dir, 
		GENERIC_READ|GENERIC_WRITE, 
		FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE, 
		NULL, 
		OPEN_EXISTING, 
		FILE_FLAG_BACKUP_SEMANTICS, 
		NULL 
		); 
	if(INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
		return -1; 
	}

	char   buf[2*(sizeof(FILE_NOTIFY_INFORMATION)+MAX_PATH)]; 
	FILE_NOTIFY_INFORMATION* pNotify=(FILE_NOTIFY_INFORMATION*)buf; 
	DWORD   BytesReturned; 
	while(true) 
	{
		if(ReadDirectoryChangesW(hFile, 
			pNotify, 
			sizeof(buf), 
			true, 
			  FILE_NOTIFY_CHANGE_FILE_NAME	//�����ļ����ĸı�
			| FILE_NOTIFY_CHANGE_DIR_NAME	//����Ŀ¼���Ƶĸı�
			//| FILE_NOTIFY_CHANGE_ATTRIBUTES	//�����ļ����Եĸı�
			//| FILE_NOTIFY_CHANGE_SIZE		//�����ļ���С�ĸı䣬ָ����д��Ӳ���ϵ��ļ����ݴ�С
			//| FILE_NOTIFY_CHANGE_LAST_WRITE	//���һ��д��ʱ��ĸı�
			//| FILE_NOTIFY_CHANGE_LAST_ACCESS//���һ�η���ʱ��ĸı�
			//| FILE_NOTIFY_CHANGE_CREATION	//�ļ�����ʱ��ĸı� 
			//| FILE_NOTIFY_CHANGE_SECURITY	//��ȫ�������ĸı�
			, 
			&BytesReturned, 
			NULL, 
			NULL)) 
		{
			//����û��Ƿ�Ҫֹͣ���̵߳ļ���
			if (pData->_bstop)
				break;
			if (BytesReturned == 0)
			{
				pushTrackInfo(_T("���"));
				continue ;
			}
			
			switch (pNotify->Action)
			{
			case FILE_ACTION_ADDED://����ļ�
				{
					WCHAR *buf = new WCHAR[sizeof(WCHAR) * pNotify->FileNameLength + 1];
					memset(buf, 0, sizeof(WCHAR) * pNotify->FileNameLength + 1);
					memcpy_s(buf, sizeof(WCHAR) * pNotify->FileNameLength + 1, pNotify->FileName, pNotify->FileNameLength);
					CString str(buf);
					if (str.Left(8).CompareNoCase(_T("RECYCLER")) != 0)
					{
						if (pData->_dir.Right(1).CompareNoCase(_T("\\")) == 0)
							str = pData->_dir + str;
						else
							str = pData->_dir + _T("\\") + str;
					}
					else
						break;
					FileMonList::getFileListObj()->addMonFilePath(str);
					delete []buf;
					buf = NULL;
					pushTrackInfo(_T("����ļ�: ") + str);
					break;
				}
			case FILE_ACTION_REMOVED://ɾ���ļ�
				{
					WCHAR *buf = new TCHAR[pNotify->FileNameLength + 1];
					memset(buf, 0, sizeof(WCHAR) * pNotify->FileNameLength + 1);
					memcpy_s(buf, sizeof(WCHAR) * pNotify->FileNameLength + 1, pNotify->FileName, pNotify->FileNameLength);
					CString str(buf);
					if (pData->_dir.Right(1).CompareNoCase(_T("\\")) == 0)
						str = pData->_dir + str;
					else
						str = pData->_dir + _T("\\") + str;
					pushTrackInfo(_T("ɾ���ļ�: ") + str);
					delete []buf;
					buf = NULL;
					break;
				}
			case FILE_ACTION_MODIFIED://�޸��ļ�
				break;
			case FILE_ACTION_RENAMED_OLD_NAME://�޸ľ��ļ���
				break;
			case FILE_ACTION_RENAMED_NEW_NAME://�޸����ļ���
				break;
			default:
				break;
			}
			//��ʱ���ù�����Ƕ�׵���Ϣ
			//if(0 != pNotify->NextEntryOffset) 
			//{
			//	PFILE_NOTIFY_INFORMATION p = (PFILE_NOTIFY_INFORMATION)((char*)pNotify+pNotify->NextEntryOffset); 
			//	pushTrackInfo(_T("�޸��ļ�"));
			//}
		} 
		else 
		{ 
			break; 
		} 
	}
	pData->_stat = MonData::CLOSED;
	CloseHandle(hFile);
	return 0;
}
void WinFileMon::addMonData(MonData* pData)
{
	EnterCriticalSection(&m_MonInfoCS);
	m_MonitorInfo.insert(pData);
	LeaveCriticalSection(&m_MonInfoCS);
}
void WinFileMon::pushTrackInfo(LPCTSTR info)
{
	WINFILEMON;
	static CString str;
	str.Empty();
	str = _T("WinFileMon: ");
	str += info;
	ProcTracker::getTracker()->pushTrackInfo(str);
}
void WinFileMon::deleteThreadInfo()
{
	EnterCriticalSection(&m_MonInfoCS);
	for (std::set<MonData*>::iterator iter = m_MonitorInfoToDel.begin(); iter != m_MonitorInfoToDel.end();)
	{
		if ((*iter)->_stat == MonData::CLOSED)
		{
			delete *iter;
			m_MonitorInfoToDel.erase(iter++);
		}
		else
			++iter;
	}
	LeaveCriticalSection(&m_MonInfoCS);
}