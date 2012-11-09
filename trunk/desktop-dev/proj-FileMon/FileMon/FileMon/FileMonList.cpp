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
	pushTrackInfo(_T("�˳��ļ������б�"));
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
	//��������ݼ�û������ɶ���Ҳû���ڵȴ����в���ӵ��ȴ�����
	if (m_FileFinishList.end() == find(m_FileFinishList.begin(), m_FileFinishList.end(), strPath))
		if (m_FileWaitList.end() == find(m_FileWaitList.begin(), m_FileWaitList.end(), strPath))
			cacheFilePath(strPath);//���Խ����ݷ���ȴ�����
	LeaveCriticalSection(&m_WaitListCS);
	return true;
}
bool FileMonList::finishOpFilePath(CString& strPath)
{
	startPhpCgi();
	//��Ϊ��������л���Ҫ��m_FileWaitList��ɾ�����ݣ�����Ҳ��Ҫ�����������ӹؼ�����ı���
	EnterCriticalSection(&m_WaitListCS);
	EnterCriticalSection(&m_finishListCS);
	bool bRes = false;
	//��Ҫȷ����·����ȷ���ڵȴ�������
	std::vector<CString>::iterator iter = find(m_FileWaitList.begin(), m_FileWaitList.end(), strPath);
	if (m_FileWaitList.end() != iter)
	{
		pushTrackInfo(_T("�ļ���") + strPath + _T("���ת��"));
		m_FileWaitList.erase(iter);
		pushTrackInfo(_T("�ļ���") + strPath + _T("�Ƴ��ȴ��б�"));
		if (checkFinishOp(strPath))//���ת��֮����ļ��Ƿ�ϸ�
		{
			pushTrackInfo(_T("�ļ���") + strPath + _T("��������б�"));
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
	//��Ҫ��ͬ��������������֧�ֲ�����ʱ������ļ��б�
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
	pushTrackInfo(_T("����ת���¼��������"));
}
bool FileMonList::getNextFileObj(CString& strFilePath)
{
	if (m_FileWaitList.empty())//����ȴ�����Ϊ�գ���ô��Ҫ����ȡ���ݵ��ȴ�������
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
//	��Ӹ�����Ϣ��Ϊ�˵���ʹ��
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
	//����Ƿ�����wmf�ļ�
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
	//�����ɶ����е�����������ָ������������ô�ͽ�����д�뵽�����ļ���
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
		pushTrackInfo(_T("��⵽����б�������������ǽ�����б�������д�뱾���ļ�ʱʧ�ܣ�"));
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
	//�����ɶ����е�����
	m_FileFinishList.clear();
}

//���ȴ��б��б���ʱ�����Խ����ݴ���ȴ����У�����ȴ�������������ô�ʹ������л������У�
//������л�������������ô�ͽ����ݱ��ػ���������SERIALIZEDFILES�ļ���
void FileMonList::cacheFilePath(CString& strFilePath)
{
	//�ú���������addMonFilePath�ڲ����õģ�����Ҫʹ�ùؼ���
	if (m_FileWaitList.size() < m_WaitListMaxSize)//�����е����޼��
	{
		pushTrackInfo(_T("���ļ���") + strFilePath + _T("����ȴ�����"));
		m_FileWaitList.push_back(strFilePath);
		//if (m_FileWaitList.size() == 1)	//ֻ�е��ȴ���������һ���ļ�����ʱ��ż����¼�
	}
	else
	{
		if (m_FileSerializeList.size() < m_SerializeListMaxSize)
		{
			pushTrackInfo(_T("���ļ���") + strFilePath + _T("���뻺�����"));
			m_FileSerializeList.push_back(strFilePath);
		}
		else//������л������е�������������ô����Ҫ����д�뱾�أ��������л�
		{
			pushTrackInfo(_T("���ļ���") + strFilePath + _T("���뻺����У�����������д���ļ�"));
			m_FileSerializeList.push_back(strFilePath);
			pushTrackInfo(_T("���������������д�뱾���ļ�serializefilenames.txt"));
			SECURITY_ATTRIBUTES sa;
			sa.nLength = sizeof(SECURITY_ATTRIBUTES);
			sa.bInheritHandle = FALSE;
			sa.lpSecurityDescriptor = NULL;
			CString strPath = GlobalFunc::getCurrentPath(SERIALIZEDFILES);
			HANDLE hFile = CreateFile(strPath, FILE_APPEND_DATA, 0, &sa, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE)
			{
				pushTrackInfo(_T("�����ļ�serializefilenames.txtʱ����"));
				SetEvent(m_hAskForConvert);//��Ҫ�����¼�
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
					buf[ansiCount] = ';';//ʹ��;�ָ�
				WriteFile(hFile, buf, ansiCount + 1, &dwByteWritten, NULL);
			}
			m_FileSerializeList.clear();
			m_bDataInFile = TRUE;
			CloseHandle(hFile);
		}
	}
	SetEvent(m_hAskForConvert);//��Ҫ�����¼�
}
//���ȴ��б��б��ʱ�����Դӱ������л��ļ���ȡ�����������еȴ�ת��������������л��ļ���û�������ˣ��ٴ����л�������ȡ������������ȴ�����
void FileMonList::fetchFilePath()
{
	EnterCriticalSection(&m_WaitListCS);
	//�ú���������finishOpFilePath�ڲ����õģ�����Ҫʹ�ùؼ���
	if (m_FileWaitList.empty())//����б�������Ϊ��
	{
		pushTrackInfo(_T("�ȴ�����Ϊ��"));
		if (m_bDataInFile)
		{
			pushTrackInfo(_T("�������ļ�serializefilenames.txt�е�����ת����ȴ�����"));
			SECURITY_ATTRIBUTES sa;
			sa.nLength = sizeof(SECURITY_ATTRIBUTES);
			sa.bInheritHandle = FALSE;
			sa.lpSecurityDescriptor = NULL;
			CString strPath = GlobalFunc::getCurrentPath(SERIALIZEDFILES);
			HANDLE hFile = CreateFile(strPath, GENERIC_READ | GENERIC_WRITE, 0, &sa, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE)
			{
				pushTrackInfo(_T("�����ļ�serializefilenames.txtʱʧ��"));
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
			//����ļ��е�����
			SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
			SetEndOfFile(hFile);
			m_bDataInFile = FALSE;
			CloseHandle(hFile);
		}
		else if (!m_FileSerializeList.empty())//�ӻ���Ķ�����ȡ����
		{
			pushTrackInfo(_T("�����������ת����ȴ�����"));
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
	pushTrackInfo(_T("������phpҳ��"));
}
void FileMonList::recvDataFromPhp(CString& strData)
{
	char buf[1000] = {0};
	DWORD dwRead = 0;
	while(ReadFile(m_hStdoutR, buf, sizeof(buf), &dwRead, NULL) && dwRead != 0){
		printf(buf);
	}
}