#include "StdAfx.h"
#include "FileMonList.h"

FileMonList* FileMonList::s_Obj = NULL;
FileMonList::FileMonList(void)
{
	m_bDataInFile = FALSE;
	m_WaitListMaxSize = MAXWAITLISTSIZE;
	m_FinishListMaxSize = MAXFINISHLISTSIZE;
	m_SerializeListMaxSize = MAXSERIALIZESIZE;
	m_strFileExt = _T("dwg");
	m_hAskForConvert = NULL;
	m_hStdoutR = NULL;
	m_hStdoutW = NULL;
	init();
}

FileMonList::~FileMonList(void)
{
	DeleteCriticalSection(&m_WaitListCS);
	DeleteCriticalSection(&m_finishListCS);
	CloseHandle(m_hAskForConvert);
	CloseHandle(m_hStdoutW);
	CloseHandle(m_hStdoutR);
	pushTrackInfo(_T("退出文件监视列表"));
}
FileMonList* FileMonList::getFileListObj()
{
	if (s_Obj == NULL)
		s_Obj = new FileMonList();
	return s_Obj;
}
void FileMonList::deleteListObj()
{
	if (s_Obj)
	{
		delete s_Obj;
		s_Obj = NULL;
	}
}

bool FileMonList::addMonFilePath(CString& strPath)
{
	if (!checkFileType(strPath))
		return false;
	strPath.Trim();
	EnterCriticalSection(&m_WaitListCS);
	//如果该数据既没有在完成队列也没有在等待队列才添加到等待队列
	if (m_FileFinishList.end() == find(m_FileFinishList.begin(), m_FileFinishList.end(), strPath))
		if (m_FileWaitList.end() == find(m_FileWaitList.begin(), m_FileWaitList.end(), strPath))
			cacheFilePath(strPath);//尝试将数据放入等待队列
	LeaveCriticalSection(&m_WaitListCS);
	return true;
}
bool FileMonList::finishOpFilePath(CString& strPath)
{
	startPhpCgi();
	//因为这个函数中会需要从m_FileWaitList中删除数据，所以也需要对这个队列添加关键区域的保护
	EnterCriticalSection(&m_WaitListCS);
	EnterCriticalSection(&m_finishListCS);
	bool bRes = false;
	//需要确定该路径的确是在等待队列中
	std::vector<CString>::iterator iter = find(m_FileWaitList.begin(), m_FileWaitList.end(), strPath);
	if (m_FileWaitList.end() != iter)
	{
		pushTrackInfo(_T("文件：") + strPath + _T("完成转换"));
		m_FileWaitList.erase(iter);
		pushTrackInfo(_T("文件：") + strPath + _T("移出等待列表"));
		if (checkFinishOp(strPath))//检查转换之后的文件是否合格
		{
			pushTrackInfo(_T("文件：") + strPath + _T("存入完成列表"));
			m_FileFinishList.push_back(strPath);
			checkFinishListSize();
			bRes = true;
		}
	}
	LeaveCriticalSection(&m_finishListCS);
	LeaveCriticalSection(&m_WaitListCS);
	sendData2Php(strPath, bRes ? _T("1") : _T("0"));
	return bRes;
}
void FileMonList::clear()
{
	//需要加同步操作，这样就支持操作的时候清空文件列表
	EnterCriticalSection(&m_WaitListCS);
	EnterCriticalSection(&m_finishListCS);
	m_FileWaitList.clear();
	m_FileFinishList.clear();
	LeaveCriticalSection(&m_finishListCS);
	LeaveCriticalSection(&m_WaitListCS);
}
void FileMonList::init()
{
	InitializeCriticalSection(&m_WaitListCS);
	InitializeCriticalSection(&m_finishListCS);
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = FALSE;
	sa.lpSecurityDescriptor = NULL;
	m_hAskForConvert = OpenEvent(EVENT_ALL_ACCESS, FALSE, CONVERT_ASKFOR);
	if (NULL == m_hAskForConvert)
		return ;
	pushTrackInfo(_T("请求转换事件创建完毕"));
}
bool FileMonList::getNextFileObj(CString& strFilePath)
{
	if (m_FileWaitList.empty())//如果等待队列为空，那么就要尝试取数据到等待队列中
	{
		fetchFilePath();
		if (m_FileWaitList.empty())
			return false;
	}

	strFilePath = m_FileWaitList[0];
	return true;
}
void FileMonList::setFileFilter(CString& strFileExt)
{
	strFileExt.Trim();
	m_strFileExt = strFileExt;
}

//Summary:
//	添加跟踪信息，为了调试使用
void FileMonList::pushTrackInfo(LPCTSTR info)
{
	FILEMONLIST;
	static CString str;
	str = _T("FileMonList: ");
	str.Append(info);
	ProcTracker::getTracker()->pushTrackInfo(str);
}

bool FileMonList::checkFileType(CString& strFilePath)
{
	strFilePath.Trim();
	static CString str;
	str = strFilePath.Right(3);
	if (str.CompareNoCase(m_strFileExt) == 0)
		return true;
	else
		return false;
}
void FileMonList::setWaitFileListMaxSize(size_t mz)
{
	m_WaitListMaxSize = mz;
}
void FileMonList::setFinishListMaxSize(size_t mz)
{
	m_FinishListMaxSize = mz;
	checkFinishListSize();
}

BOOL FileMonList::checkFinishOp(CString strParam)
{
	//检查是否生成wmf文件
	char buf[MAX_PATH] = {0};
	int pos = strParam.ReverseFind(_T('.'));
	if (pos == -1)
		return FALSE;
	strParam = strParam.Left(pos + 1) + _T("emf");
	if (GlobalFunc::UnicodeToANSI(strParam, buf, MAX_PATH))
	{
		//OFSTRUCT of;
		//ZeroMemory(&of, sizeof(OFSTRUCT));
		//of.cBytes = sizeof(OFSTRUCT);
		//return ((HFILE_ERROR != OpenFile(buf, &of, OF_EXIST)) && (GlobalFunc::getFileSize(CString(buf)) > 0));
		if (GlobalFunc::getFileSize(CString(buf)) == 0)
			DeleteFile(CString(buf));
		else 
			return TRUE;
	}
	return FALSE;
}


void FileMonList::checkFinishListSize()
{
	if (m_FileFinishList.size() < m_FinishListMaxSize)
		return ;
	//如果完成队列中的数据量超过指定的数量，那么就将数据写入到本地文件中
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = FALSE;
	sa.lpSecurityDescriptor = NULL;
	
	CString strPath;
	TCHAR curPath[MAX_PATH] = {0};
	if (GetModuleFileName(NULL, curPath, MAX_PATH) != 0)
	{
		strPath = curPath;
		strPath = strPath.Left(strPath.ReverseFind(_T('\\')) + 1) + FINISHLISTHISTORY;
	}
	HANDLE hFile = CreateFile(strPath, FILE_APPEND_DATA, 0, &sa, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		pushTrackInfo(_T("检测到完成列表超出额定数量，但是将完成列表中数据写入本地文件时失败！"));
		return ;
	}
	DWORD dwByteWritten = 0;
	CHAR buf[W2FBUFSIZE] = {0};
	CString strAnsi;
	int ansiCount;
	for (std::vector<CString>::iterator iter = m_FileFinishList.begin(); iter != m_FileFinishList.end(); ++iter)
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
	//清空完成队列中的数据
	m_FileFinishList.clear();
}

//当等待列表中变满时，尝试将数据存入等待队列，如果等待队列已满，那么就存入序列化队列中，
//如果序列化队列已满，那么就将数据本地化，保存至SERIALIZEDFILES文件中
void FileMonList::cacheFilePath(CString& strFilePath)
{
	//该函数由于是addMonFilePath内部调用的，不需要使用关键段
	if (m_FileWaitList.size() < m_WaitListMaxSize)//做队列的上限检查
	{
		pushTrackInfo(_T("将文件名") + strFilePath + _T("存入等待队列"));
		m_FileWaitList.push_back(strFilePath);
		//if (m_FileWaitList.size() == 1)	//只有当等待队列中有一个文件名的时候才激活事件
	}
	else
	{
		if (m_FileSerializeList.size() < m_SerializeListMaxSize)
		{
			pushTrackInfo(_T("将文件名") + strFilePath + _T("存入缓存队列"));
			m_FileSerializeList.push_back(strFilePath);
		}
		else//如果序列化队列中的数据已满，那么就需要将其写入本地，进行序列化
		{
			pushTrackInfo(_T("将文件名") + strFilePath + _T("存入缓存队列，但是已满，写入文件"));
			m_FileSerializeList.push_back(strFilePath);
			pushTrackInfo(_T("将缓冲队列中数据写入本地文件serializefilenames.txt"));
			SECURITY_ATTRIBUTES sa;
			sa.nLength = sizeof(SECURITY_ATTRIBUTES);
			sa.bInheritHandle = FALSE;
			sa.lpSecurityDescriptor = NULL;
			CString strPath = GlobalFunc::getCurrentPath(SERIALIZEDFILES);
			HANDLE hFile = CreateFile(strPath, FILE_APPEND_DATA, 0, &sa, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE)
			{
				pushTrackInfo(_T("创建文件serializefilenames.txt时出错"));
				SetEvent(m_hAskForConvert);//需要激活事件
				return ;
			}
			DWORD dwByteWritten = 0;
			CHAR buf[W2FBUFSIZE] = {0};
			CString strAnsi;
			int ansiCount;
			for (std::vector<CString>::iterator iter = m_FileSerializeList.begin(); iter != m_FileSerializeList.end(); ++iter)
			{
				ZeroMemory(buf, W2FBUFSIZE);
				strAnsi = *iter;
				ansiCount = GlobalFunc::UnicodeToANSI(strAnsi, buf, W2FBUFSIZE);
				if (iter->GetLength() < BUFSIZE - 1)
					buf[ansiCount] = ';';//使用;分割
				WriteFile(hFile, buf, ansiCount + 1, &dwByteWritten, NULL);
			}
			m_FileSerializeList.clear();
			m_bDataInFile = TRUE;
			CloseHandle(hFile);
		}
	}
	SetEvent(m_hAskForConvert);//需要激活事件
}
//当等待列表中变空时，尝试从本地序列化文件中取出数据来进行等待转化，如果本地序列化文件中没有数据了，再从序列化队列中取出数据来放入等待队列
void FileMonList::fetchFilePath()
{
	EnterCriticalSection(&m_WaitListCS);
	//该函数由于是finishOpFilePath内部调用的，不需要使用关键段
	if (m_FileWaitList.empty())//如果列表中数据为空
	{
		pushTrackInfo(_T("等待队列为空"));
		if (m_bDataInFile)
		{
			pushTrackInfo(_T("将本地文件serializefilenames.txt中的数据转存入等待队列"));
			SECURITY_ATTRIBUTES sa;
			sa.nLength = sizeof(SECURITY_ATTRIBUTES);
			sa.bInheritHandle = FALSE;
			sa.lpSecurityDescriptor = NULL;
			CString strPath = GlobalFunc::getCurrentPath(SERIALIZEDFILES);
			HANDLE hFile = CreateFile(strPath, GENERIC_READ | GENERIC_WRITE, 0, &sa, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE)
			{
				pushTrackInfo(_T("创建文件serializefilenames.txt时失败"));
				LeaveCriticalSection(&m_WaitListCS);
				return ;
			}
			DWORD dwByteWritten = 0;
			CHAR buf[W2FBUFSIZE] = {0};
			CString strCache, strTemp;
			int pos = 0;
			DWORD dwReadBytes = 0;
			while (1)
			{
				ZeroMemory(buf, W2FBUFSIZE);
				if (ReadFile(hFile, buf, W2FBUFSIZE * sizeof(CHAR), &dwReadBytes, NULL) == TRUE)
				{
					if (dwReadBytes == 0)
						break;
					strTemp = buf;
					strCache.Append(strTemp);
					pos = strCache.Find(_T(';'));
					while (pos > -1)
					{
						strTemp = strCache.Left(pos);
						strCache = strCache.Right(strCache.GetLength() - pos - 1);
						m_FileWaitList.push_back(strTemp);
						pos = strCache.Find(_T(';'));
					}
				}
			}
			//清空文件中的数据
			SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
			SetEndOfFile(hFile);
			m_bDataInFile = FALSE;
			CloseHandle(hFile);
		}
		else if (!m_FileSerializeList.empty())//从缓存的队列中取数据
		{
			pushTrackInfo(_T("将缓存的数据转存入等待队列"));
			m_FileWaitList.assign(m_FileSerializeList.begin(), m_FileSerializeList.end());
			m_FileSerializeList.clear();
		}
	}
	LeaveCriticalSection(&m_WaitListCS);
}
void FileMonList::startPhpCgi()
{
	if (m_hStdoutR != NULL && m_hStdoutW != NULL)
		return ;
	SECURITY_ATTRIBUTES sa = {0};
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = 1;
	sa.lpSecurityDescriptor = NULL;

	CreatePipe(&m_hStdoutR, &m_hStdoutW, &sa, 0);
	SetHandleInformation(m_hStdoutW, HANDLE_FLAG_INHERIT, 0);
	
	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi;
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdOutput = m_hStdoutW;
	si.hStdInput = m_hStdoutR;

	char env[255] = "REQUEST_METHOD=POST\0CONTENT_LENGTH=18\0CONTENT_TYPE=application/x-www-form-urlencoded\0SCRIPT_FILENAME=F:\\backup\\test.html";
	TCHAR cmdLine[MAX_PATH] = _T("F:\\backup\\php5\\php-cgi.exe F:\\backup\\test.html");
	if(!CreateProcess(NULL, cmdLine,
		NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, env, NULL, &si, &pi))
		return ;
}

void FileMonList::sendData2Php(CString strFilePath, CString strRes)
{
	DWORD dwWritten = 0;
	static char buf[2048] = {0};
	ZeroMemory(buf, 2048);
	static CString strSend = _T("filelName=") + strFilePath + _T(";convRes=") + strRes;
	GlobalFunc::UnicodeToANSI(strSend, buf, 2048);
	if(!WriteFile(m_hStdoutW, buf, 2048, &dwWritten, NULL))
		return ;
	pushTrackInfo(_T("调用了php页面"));
}
void FileMonList::recvDataFromPhp(CString& strData)
{
	char buf[1000] = {0};
	DWORD dwRead = 0;
	while(ReadFile(m_hStdoutR, buf, sizeof(buf), &dwRead, NULL) && dwRead != 0){
		printf(buf);
	}
}