#pragma once
#include <afxinet.h>

#define HOSTNAME		_T("www.hao2580.com")
#define DWGHANDLEPAGE	_T("ReciveResult.php")
#define PDFHOSTNAME		_T("dl.hao2580.com")
#define PDFHANDLEPAGE	_T("tpdf/php/services/view2.php")


//这个类负责向php传送数据，使用的是HTTP协议
class PostMsg2Php
{
public:
	enum ActionRes{
		Failed = 0,//操作失败
		Suc = 1,//操作成功
	};
	enum DwgLocation{
		TMGLinux = 0,//在天极的Linux服务器上
		AECWindows = 1,//在AEC的Windows服务器上
		AECLinux = 2,//AEC的Linux服务器上
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

	//向php页面传送数据，这个函数处理的是DWG文件在转化完成之后，上传到天极FTP服务器之后的数据通信
	PostDataRes PostResult(CString& strFileName, Action act, ActionRes nRes)
	{
		static CInternetSession sess;//建立会话 
		static CString strServer = HOSTNAME;
		static CString strHandlePage = DWGHANDLEPAGE;
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
			char *pElementText = new char[iTextLen];
			memset( ( void* )pElementText, 0, sizeof( char ) * iTextLen );
			::WideCharToMultiByte( CP_ACP,
				0,
				strData.GetBuffer(),
				-1,
				pElementText,
				iTextLen,
				NULL,
				NULL );
			strData.ReleaseBuffer();
			BOOL bRes = pFile->SendRequest(strHeaders, (LPVOID)pElementText, iTextLen);
			delete pElementText;
			pElementText = NULL;

			DWORD retcode;
			pFile->QueryInfoStatusCode(retcode);
			char szServerMsg[1024] = {0};
			//读取服务器返回的结果
			UINT nSize = pFile->Read(szServerMsg, 1024);
			DWORD dwFileSize = 0;
			while (nSize > 0)
			{
				dwFileSize += nSize;
				nSize = pFile->Read(szServerMsg + nSize, 1024);
			}
			szServerMsg[dwFileSize] = '\0';//插入结束标志
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
			char postdatasuc[]="1";
			if (memcmp(szServerMsg, postdatasuc, sizeof(postdatasuc)) == 0)
				pushTrackInfo(_T("向php发送数据成功 : ") + strFileName);

			//sess.Close();
			if (bRes)
				pushTrackInfo(_T("添加操作结果成功 : ") + strFileName);
			else
				pushTrackInfo(_T("返回操作结果失败 : ") + strFileName);
		}
		catch (...)
		{
			return OpSuc;
		}
		return OpSuc;
	}
	//将pdf文件信息按照
	PostDataRes PostDataForPdfFile(CString strPdfFilePath)
	{
		static CInternetSession sess;//建立会话 
		static CString strServer = PDFHOSTNAME;
		static CString strHandlePage = PDFHANDLEPAGE;
		INTERNET_PORT nPort = 80;
		try
		{
			CHttpConnection* pServer = sess.GetHttpConnection(strServer, nPort);
			CHttpFile *pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, strHandlePage, NULL, 1, NULL, NULL);

			CString strHeaders = _T("Content-Type: application/x-www-form-urlencoded"); 
			CString strData;
			strData.Format(_T("doc=%s"), strPdfFilePath);
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
			char *pElementText = new char[iTextLen];
			memset( ( void* )pElementText, 0, sizeof( char ) * iTextLen);
			::WideCharToMultiByte( CP_ACP,
				0,
				strData.GetBuffer(),
				-1,
				pElementText,
				iTextLen,
				NULL,
				NULL );
			strData.ReleaseBuffer();
			BOOL bRes = pFile->SendRequest(strHeaders, (LPVOID)pElementText, iTextLen-1);
			delete pElementText;
			pElementText = NULL;

			DWORD retcode;
			pFile->QueryInfoStatusCode(retcode);
			char szServerMsg[1024] = {0};
			//读取服务器返回的结果
			UINT nSize = pFile->Read(szServerMsg, 1024);
			DWORD dwFileSize = 0;
			while (nSize > 0)
			{
				dwFileSize += nSize;
				nSize = pFile->Read(szServerMsg + nSize, 1024);
			}
			szServerMsg[dwFileSize] = '\0';//插入结束标志
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
				pushTrackInfo(_T("检测到PDF文件，通知PHP页面 成功 : ") + strPdfFilePath);
			else
				pushTrackInfo(_T("检测到PDF文件，通知PHP页面 失败 : ") + strPdfFilePath);
		}
		catch (...)
		{
			return OpSuc;
		}
		return OpSuc;
	}
	//Summary:
	//	添加跟踪信息，为了调试使用
	void pushTrackInfo(LPCTSTR info)
	{
		POSTMSG2PHP;
		static CString str;
		str = _T("PostMsg2Php: ");
		str.Append(info);
		ProcTracker::getTracker()->pushTrackInfo(str);
	}
};