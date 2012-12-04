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

//�õ����ȶ���
ConvertManager* ConvertManager::getConvertManagerObj()
{
	if (s_ConvertManager == NULL)
		s_ConvertManager = new ConvertManager();
	return s_ConvertManager;
}
//ɾ�����ȶ���
void ConvertManager::deleteConvertManager()
{
	if (s_ConvertManager)
		delete s_ConvertManager;
	s_ConvertManager = NULL;
}
//��ʼ����
void ConvertManager::startManager()
{
	pushTrackInfo(_T("���������߳�"));
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
//�����̵߳�������
DWORD WINAPI ConvertManager::ManagerThread(LPVOID param)
{
	pushTrackInfo(_T("�����߳̿�ʼִ��"));
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
			pushTrackInfo(_T("�����߳��˳�: ��������ת���¼�ʧ��"));
			return -1;
		}
	}

	if (s_hBeginConvert == NULL)
	{
		s_hBeginConvert = CreateEvent(&sa, TRUE, FALSE, CONVERT_BEGIN);
		if (NULL == s_hBeginConvert)
		{
			pushTrackInfo(_T("�����߳��˳�: ������ʼת���¼�ʧ��"));
			return -1;
		}
	}

	if (s_hFinishConvert == NULL)
	{
		s_hFinishConvert = CreateEvent(&sa, TRUE, FALSE, CONVERT_FINISH);
		if (NULL == s_hFinishConvert)
		{
			pushTrackInfo(_T("�����߳��˳�: ����ת������¼�ʧ��"));
			return -1;
		}
	}

	if (s_hPipe == NULL)
	{
		//���������ܵ�
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
			pushTrackInfo(_T("���������ܵ�ʧ��"));
			return -1;
		}
		pushTrackInfo(_T("���������ܵ��ɹ�"));
		pushTrackInfo(_T("�ȴ��ͻ������ӹܵ�"));
		BOOL bConnect = ConnectNamedPipe(s_hPipe, NULL)? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
		if (!bConnect)
		{
			pushTrackInfo(_T("�ͻ������ӹܵ�ʧ��"));
			DisconnectNamedPipe(s_hPipe);
			CloseHandle(s_hPipe);
			return -1;
		}
	}
	TCHAR buf[BUFSIZE] = {0};
	DWORD dwWrittenByte = 0, dwRealBufSize = 0;
	while (1)
	{
		pushTrackInfo(_T("�ȴ����ʼת���¼�"));
		//�ȴ������¼�
		WaitForSingleObject(s_hAskForConvert, INFINITE);
		pushTrackInfo(_T("���ʼת���¼�"));
		//ȡ�ô�ת�������е��ļ��б�
		CString str;
		bool bHasFile = FileMonList::getFileListObj()->getNextFileObj(str);
		while (bHasFile)
		{
			//���ʼת���¼�
			SetEvent(s_hBeginConvert);
			pushTrackInfo(_T("���ӹܵ�"));

			//����ת����������Ҫ�Ĳ���
			//��strд�����ݻ�����
			ZeroMemory(buf, BUFSIZE*sizeof(TCHAR));
			wcscpy_s(buf, BUFSIZE, str);
			dwRealBufSize = str.GetLength() * sizeof(TCHAR);
			pushTrackInfo(_T("д��ܵ�����"));
			//���Ӻ�֮��Ϳ�ʼд����
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
			pushTrackInfo(_T("�ȴ�ת������"));

			//�ȴ�ת������¼�
			WaitForSingleObject(s_hFinishConvert, INFINITE);//TODO���������Ҫ����ʵ����Ҫ����һ����ת���¼�
			ResetEvent(s_hFinishConvert);//��ԭת������¼�
			FlushFileBuffers(s_hPipe);//�������
			//�����ļ��б������ݱ�ʾ�Ѿ�ת�����
			FileMonList::getFileListObj()->finishOpFilePath(str);

			pushTrackInfo(_T("�˴�ת������"));
			PostResult(str, true);
			//ȡ����һ����ת���ļ�·��
			bHasFile = FileMonList::getFileListObj()->getNextFileObj(str);
		}
		ResetEvent(s_hAskForConvert);//�����ǰת���������ͽ�����һ��ת���ȴ�
	}
	DisconnectNamedPipe(s_hPipe);
	CloseHandle(s_hPipe);
	return 0;
}
//Summary
//  ����ת�����
void ConvertManager::PostResult(CString& strFileName, bool bSuc)
{
	static CInternetSession sess;//�����Ự 
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
		pushTrackInfo(_T("��Ӳ�������ɹ� : ") + strFileName);
	else
		pushTrackInfo(_T("���ز������ʧ�� : ") + strFileName);
}
//Summary:
//	��Ӹ�����Ϣ��Ϊ�˵���ʹ��
void ConvertManager::pushTrackInfo(LPCTSTR info)
{
	CONVERTMANAGER;
	static CString str;
	str = _T("ConvertManager: ");
	str.Append(info);
	ProcTracker::getTracker()->pushTrackInfo(str);
}
