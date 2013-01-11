#pragma once

#define HOSTNAME	_T("www.hao2580.com")
#define HANDLEPAGE	_T("ReciveResult.php")


//����ฺ����php��������
class PostMsg2Php
{
public:
	enum ActionRes{
		Failed = 0,//����ʧ��
		Suc = 1,//�����ɹ�
	};
	enum DwgLocation{
		TMGLinux = 0,//���켫��Linux��������
		AECWindows = 1,//��AEC��Windows��������
		AECLinux = 2,//AEC��Linux��������
	};
	enum PostDataRes{
		OpSuc = 0,
		ParameterError = 1,
		PostDataFailed = 2,
	};
	enum Action{
		Convert = 0,
		Upload = 1
	};

	//��phpҳ�洫�����ݣ�
	PostDataRes PostResult(CString& strFileName, Action act, ActionRes nRes)
	{
		static CInternetSession sess;//�����Ự 
		static CString strServer = HOSTNAME;
		static CString strHandlePage = HANDLEPAGE;
		INTERNET_PORT nPort = 80;
		try
		{
			CHttpConnection* pServer = sess.GetHttpConnection(strServer, nPort);
			CHttpFile *pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, strHandlePage, NULL, 1, NULL, NULL);

			CString strHeaders = _T("Content-Type: application/x-www-form-urlencoded"); 
			CString strData;
			if (act == Convert)
				strData.Format(_T("FilePath=%s&Act=Convert&wmf_fini=%d&action=submit"), strFileName, nRes);
			else if (act == Upload)
				strData.Format(_T("FilePath=%s&Act=Upload&wmf_fini=%d&action=submit"), strFileName, nRes);
			int    iTextLen;
			// wide char to multi char
			iTextLen = WideCharToMultiByte( CP_ACP,
				0,
				strData.GetBuffer(),
				-1,
				NULL,
				0,
				NULL,
				NULL );
			strData.ReleaseBuffer();
			char *pElementText = new char[iTextLen + 1];
			memset( ( void* )pElementText, 0, sizeof( char ) * ( iTextLen + 1 ) );
			::WideCharToMultiByte( CP_ACP,
				0,
				strData.GetBuffer(),
				-1,
				pElementText,
				iTextLen,
				NULL,
				NULL );
			strData.ReleaseBuffer();
			BOOL bRes = pFile->SendRequest(strHeaders, (LPVOID)pElementText, strData.GetLength());
			delete pElementText;
			pElementText = NULL;

			DWORD retcode;
			pFile->QueryInfoStatusCode(retcode);
			char szServerMsg[1024] = {0};
			//��ȡ���������صĽ��
			UINT nSize = pFile->Read(szServerMsg, 1024);
			DWORD dwFileSize = 0;
			while (nSize > 0)
			{
				dwFileSize += nSize;
				nSize = pFile->Read(szServerMsg + nSize, 1024);
			}
			szServerMsg[dwFileSize] = '\0';//���������־
			char ParameterErr[] = "Parameter error";
			char postdatafailed[] = "0";
			if (memcmp(szServerMsg, ParameterErr, sizeof(ParameterErr)) == 0)
			{
				return ParameterError;
			}
			else if (memcmp(szServerMsg, postdatafailed, sizeof(postdatafailed)) == 0)
			{
				return PostDataFailed;
			}

			pFile->Close();
			pServer->Close();
			delete pFile;
			delete pServer;
			//sess.Close();
			if (bRes)
				pushTrackInfo(_T("��Ӳ�������ɹ� : ") + strFileName);
			else
				pushTrackInfo(_T("���ز������ʧ�� : ") + strFileName);
		}
		catch (...)
		{
			return OpSuc;
		}
		return OpSuc;
	}
	//Summary:
	//	��Ӹ�����Ϣ��Ϊ�˵���ʹ��
	void pushTrackInfo(LPCTSTR info)
	{
		POSTMSG2PHP;
		static CString str;
		str = _T("PostMsg2Php: ");
		str.Append(info);
		ProcTracker::getTracker()->pushTrackInfo(str);
	}
};