#include "StdAfx.h"
#include "ConvertManager.h"
#include "FileMonList.h"
#include <afxinet.h>

ConvertManager* ConvertManager::s_ConvertManager = NULL;
HANDLE ConvertManager::s_hAskForConvert = NULL;
HANDLE ConvertManager::s_hBeginConvert = NULL;
HANDLE ConvertManager::s_hFinishConvert = NULL;
HANDLE ConvertManager::s_hPipe = NULL;


ConvertManager::ConvertManager(void)
{
	m_bStartThread = false;
}

ConvertManager::~ConvertManager(void)
{
	CloseHandle(s_hAskForConvert);
	SetEvent(s_hBeginConvert);
	CloseHandle(s_hBeginConvert);
	CloseHandle(s_hFinishConvert);
}

//得到调度对象
ConvertManager* ConvertManager::getConvertManagerObj()
{
	if (s_ConvertManager == NULL)
		s_ConvertManager = new ConvertManager();
	return s_ConvertManager;
}
//删除调度对象
void ConvertManager::deleteConvertManager()
{
	if (s_ConvertManager)
		delete s_ConvertManager;
	s_ConvertManager = NULL;
}
//开始调度
void ConvertManager::startManager()
{
	pushTrackInfo(_T("开启调度线程"));
	if (m_bStartThread)
		return ;
	m_bStartThread = true;
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = FALSE;
	sa.lpSecurityDescriptor = NULL;
	DWORD dwThrwadId = 0;
	HANDLE hThread = CreateThread(&sa, 0, ManagerThread, NULL, 0, &dwThrwadId);
	if (hThread == NULL)
	{
		m_bStartThread = false;
		return ;
	}
}
//调度线程的主程序
DWORD WINAPI ConvertManager::ManagerThread(LPVOID param)
{
	pushTrackInfo(_T("调度线程开始执行"));
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = FALSE;
	sa.lpSecurityDescriptor = NULL;
	if (s_hAskForConvert == NULL)
	{
		//s_hAskForConvert = OpenEvent(EVENT_ALL_ACCESS, FALSE, CONVERT_ASKFOR);
		s_hAskForConvert = CreateEvent(&sa, TRUE, FALSE, CONVERT_ASKFOR);
		if (NULL == s_hAskForConvert)
		{
			pushTrackInfo(_T("调度线程退出: 创建请求转换事件失败"));
			return -1;
		}
	}

	if (s_hBeginConvert == NULL)
	{
		s_hBeginConvert = CreateEvent(&sa, TRUE, FALSE, CONVERT_BEGIN);
		if (NULL == s_hBeginConvert)
		{
			pushTrackInfo(_T("调度线程退出: 创建开始转换事件失败"));
			return -1;
		}
	}

	if (s_hFinishConvert == NULL)
	{
		s_hFinishConvert = CreateEvent(&sa, TRUE, FALSE, CONVERT_FINISH);
		if (NULL == s_hFinishConvert)
		{
			pushTrackInfo(_T("调度线程退出: 创建转换完毕事件失败"));
			return -1;
		}
	}

	if (s_hPipe == NULL)
	{
		//创建命名管道
		s_hPipe = CreateNamedPipe(
			PIPE_M2CPARAMETER, 
			PIPE_ACCESS_DUPLEX, 
			PIPE_TYPE_MESSAGE | 
			PIPE_READMODE_MESSAGE | PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES,
			BUFSIZE,
			BUFSIZE,
			0,
			&sa);
		if (s_hPipe == INVALID_HANDLE_VALUE)
		{
			pushTrackInfo(_T("创建命名管道失败"));
			return -1;
		}
		pushTrackInfo(_T("创建命名管道成功"));
		pushTrackInfo(_T("等待客户端连接管道"));
		BOOL bConnect = ConnectNamedPipe(s_hPipe, NULL)? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
		if (!bConnect)
		{
			pushTrackInfo(_T("客户端连接管道失败"));
			DisconnectNamedPipe(s_hPipe);
			CloseHandle(s_hPipe);
			return -1;
		}
	}
	TCHAR buf[BUFSIZE] = {0};
	DWORD dwWrittenByte = 0, dwRealBufSize = 0;
	while (1)
	{
		pushTrackInfo(_T("等待激活开始转换事件"));
		//等待唤醒事件
		WaitForSingleObject(s_hAskForConvert, INFINITE);
		pushTrackInfo(_T("激活开始转换事件"));
		//取得待转换队列中的文件列表
		CString str;
		bool bHasFile = FileMonList::getFileListObj()->getNextFileObj(str);
		while (bHasFile)
		{
			//激活开始转换事件
			SetEvent(s_hBeginConvert);
			pushTrackInfo(_T("连接管道"));

			//设置转换程序中需要的参数
			//将str写到数据缓冲区
			ZeroMemory(buf, BUFSIZE*sizeof(TCHAR));
			wcscpy_s(buf, BUFSIZE, str);
			dwRealBufSize = str.GetLength() * sizeof(TCHAR);
			pushTrackInfo(_T("写入管道数据"));
			//连接好之后就开始写数据
			BOOL bConnect = WriteFile(s_hPipe, 
				buf,
				dwRealBufSize,
				&dwWrittenByte,
				NULL
				);
			if (!bConnect)
				break;
			if (!bConnect && dwWrittenByte != dwRealBufSize)
				break;
			pushTrackInfo(buf);
			pushTrackInfo(_T("等待转换结束"));

			//等待转换完毕事件
			WaitForSingleObject(s_hFinishConvert, INFINITE);//TODO：这个可能要根据实际需要设置一定的转换事件
			ResetEvent(s_hFinishConvert);//还原转换完毕事件
			FlushFileBuffers(s_hPipe);//清空数据
			//设置文件列表中数据表示已经转化完毕
			FileMonList::getFileListObj()->finishOpFilePath(str);

			pushTrackInfo(_T("此次转换结束"));
			PostResult(str, true);
			//取得下一个待转换文件路径
			bHasFile = FileMonList::getFileListObj()->getNextFileObj(str);
		}
		ResetEvent(s_hAskForConvert);//如果当前转换结束，就进行下一次转换等待
	}
	DisconnectNamedPipe(s_hPipe);
	CloseHandle(s_hPipe);
	return 0;
}
//Summary
//  传输转换结果
void ConvertManager::PostResult(CString& strFileName, bool bSuc)
{
	static CInternetSession sess;//建立会话 
	static CString strUrl = _T("");
	static CString strServer = _T("localhost");
	static CString strHandlePage = _T("/testpost.php");
	CHttpConnection* pServer = sess.GetHttpConnection(strServer);
	CHttpFile *pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, strHandlePage, NULL, 1, NULL, NULL);

	CString strHeaders = _T("Content-Type: application/x-www-form-urlencoded"); 
	CString strData;
	strData.Format(_T("FilePath=%s&Result=%s"), strFileName, bSuc ? _T("1") : _T("0"));
	BOOL bRes = pFile->SendRequest(strHeaders, (LPVOID)(LPCTSTR)strData, strData.GetLength());
	if (bRes)
		pushTrackInfo(_T("添加操作结果成功 : ") + strFileName);
	else
		pushTrackInfo(_T("返回操作结果失败 : ") + strFileName);
}
//Summary:
//	添加跟踪信息，为了调试使用
void ConvertManager::pushTrackInfo(LPCTSTR info)
{
	CONVERTMANAGER;
	static CString str;
	str = _T("ConvertManager: ");
	str.Append(info);
	ProcTracker::getTracker()->pushTrackInfo(str);
}
