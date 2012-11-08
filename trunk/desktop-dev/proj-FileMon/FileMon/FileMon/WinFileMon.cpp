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
	pushTrackInfo(_T("退出文件监视"));
}

//添加要监视的文件夹
bool WinFileMon::addMonitorDir(CString strDir)
{
	std::vector<CString> vDir;
	int res = checkDir(strDir, vDir);
	if (res == 0)
	{
		pushTrackInfo(_T("添加路径: ") + strDir);
		return startMonitorThread(strDir);
	}
	else if (res == 2)
	{
		//需要将指定的文件夹相关文件取消监视
		for (std::vector<CString>::iterator iter = vDir.begin(); iter != vDir.end(); ++iter)
			removeMonitorDir(*iter);
		//之后再启使用新的监视
		pushTrackInfo(_T("添加路径: ") + strDir);
		return startMonitorThread(strDir);
	}
	else//1, 3 不予处理
		;
	return false;
}
//删除要监视的文件夹
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
			pushTrackInfo(_T("停止监视路径: ") + strDir);
			break;
		}
	}
	LeaveCriticalSection(&m_MonInfoCS);
	//std::set<MonData>::iterator iter = find(m_MonitorInfo.begin(), m_MonitorInfo.end(), );
}
//开启新的监视线程
bool WinFileMon::startMonitorThread(CString strDir)
{
	//首先尝试看能否清空一部分已经终止的线程的相关参数
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
		pushTrackInfo(_T("开始监视路径: ") + pData->_dir);
		return true;
	}
	else
	{
		//没有成功就要删除new出来的监控数据
		delete pData;
		pData = NULL;
		return false;
	}
}

//需要检查监视的文件夹是否已经在监视的文件夹或者子文件夹中，或者是包含了其中的一些文件夹
int WinFileMon::checkDir(CString strDir, std::vector<CString>& dirs)
{
	//检查给定的路径是否是一个已经存在的文件夹
	//GetFileAttributes()
	if (!PathFileExists(strDir))
		return 3;
	int res = 0;	//返回结果
	int len = strDir.GetLength();
	CString strTmp;
	EnterCriticalSection(&m_MonInfoCS);
	//检查给定的文件夹路径是否是其他文件夹的父目录，也就是检查给定的strDir是否是m_Dirs中的某些文件夹路径的前缀
	for (std::set<MonData*>::iterator iter = m_MonitorInfo.begin(); iter != m_MonitorInfo.end(); ++iter)
	{
		if ((*iter)->_dir.GetLength() < len)
			continue;
		strTmp = (*iter)->_dir.Left(len);
		if (strTmp.CompareNoCase(strDir) == 0)
		{
			dirs.push_back((*iter)->_dir);//将所有给定目录的子目录路径取出来
			res = 2;
		}
	}

	//接下来检查是否被包含于已经处于监视的文件夹中，也就是检查是否有已经处于监视的文件夹路径作为给定文件夹路径的前缀
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
//监视代码
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
			  FILE_NOTIFY_CHANGE_FILE_NAME	//监视文件名的改变
			| FILE_NOTIFY_CHANGE_DIR_NAME	//监视目录名称的改变
			//| FILE_NOTIFY_CHANGE_ATTRIBUTES	//监视文件属性的改变
			//| FILE_NOTIFY_CHANGE_SIZE		//监视文件大小的改变，指的是写到硬盘上的文件数据大小
			//| FILE_NOTIFY_CHANGE_LAST_WRITE	//最后一次写入时间的改变
			//| FILE_NOTIFY_CHANGE_LAST_ACCESS//最后一次访问时间的改变
			//| FILE_NOTIFY_CHANGE_CREATION	//文件创建时间的改变 
			//| FILE_NOTIFY_CHANGE_SECURITY	//安全描述符的改变
			, 
			&BytesReturned, 
			NULL, 
			NULL)) 
		{
			//检查用户是否要停止该线程的监视
			if (pData->_bstop)
				break;
			if (BytesReturned == 0)
			{
				pushTrackInfo(_T("溢出"));
				continue ;
			}
			
			switch (pNotify->Action)
			{
			case FILE_ACTION_ADDED://添加文件
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
					pushTrackInfo(_T("添加文件: ") + str);
					break;
				}
			case FILE_ACTION_REMOVED://删除文件
				{
					WCHAR *buf = new TCHAR[pNotify->FileNameLength + 1];
					memset(buf, 0, sizeof(WCHAR) * pNotify->FileNameLength + 1);
					memcpy_s(buf, sizeof(WCHAR) * pNotify->FileNameLength + 1, pNotify->FileName, pNotify->FileNameLength);
					CString str(buf);
					if (pData->_dir.Right(1).CompareNoCase(_T("\\")) == 0)
						str = pData->_dir + str;
					else
						str = pData->_dir + _T("\\") + str;
					pushTrackInfo(_T("删除文件: ") + str);
					delete []buf;
					buf = NULL;
					break;
				}
			case FILE_ACTION_MODIFIED://修改文件
				break;
			case FILE_ACTION_RENAMED_OLD_NAME://修改旧文件名
				break;
			case FILE_ACTION_RENAMED_NEW_NAME://修改新文件名
				break;
			default:
				break;
			}
			//暂时不用管里面嵌套的信息
			//if(0 != pNotify->NextEntryOffset) 
			//{
			//	PFILE_NOTIFY_INFORMATION p = (PFILE_NOTIFY_INFORMATION)((char*)pNotify+pNotify->NextEntryOffset); 
			//	pushTrackInfo(_T("修改文件"));
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