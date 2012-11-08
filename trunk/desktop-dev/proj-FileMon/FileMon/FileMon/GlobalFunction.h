#pragma once

class GlobalFunc
{
public:
	static int UnicodeToANSI(const CString& str, char* pTarget, int maxTagertSize)
	{
		char* pElementText;
		int	iTextLen;
		// wide char to multi char
		iTextLen = WideCharToMultiByte(CP_ACP,
			0,
			(LPCTSTR)str,
			-1,
			NULL,
			0,
			NULL,
			NULL);

		pElementText = new char[iTextLen + 1];
		memset((void*)pElementText,0,sizeof(char)*(iTextLen+1));

		::WideCharToMultiByte( CP_ACP,
			0,
			(LPCTSTR)str,
			-1,
			pElementText,
			iTextLen,
			NULL,
			NULL);

		if (iTextLen < maxTagertSize)
			memcpy_s(pTarget, maxTagertSize, pElementText, iTextLen);
		else
		{
			delete [] pElementText;
			return 0;
		}

		delete[] pElementText;
		return iTextLen-1;
	}
	//该函数返回值为 当前程序运行路径+给定文件名
	static CString getCurrentPath(LPCTSTR strFileName = NULL)
	{
		WCHAR path[MAX_PATH] = {0};
		GetModuleFileName(NULL, path, MAX_PATH);
		CString curDir = path;
		curDir = curDir.Left(curDir.ReverseFind(_T('\\')) + 1) + strFileName;
		return curDir;
	}
	//将GetLastError转化为文字描述
	static CString formatErrorCode(DWORD dwErr)
	{
		CString strErr;
		LPVOID lpMsgBuf;
		FormatMessage(  
			FORMAT_MESSAGE_ALLOCATE_BUFFER |   
			FORMAT_MESSAGE_FROM_SYSTEM |  
			FORMAT_MESSAGE_IGNORE_INSERTS,  
			NULL,  
			dwErr,  
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),  
			(LPTSTR)&lpMsgBuf,
			64, NULL );
		return strErr = (LPTSTR)&lpMsgBuf;
	}
	static BOOL DirectoryExist(CString strPath)
	{
		WIN32_FIND_DATA wfd;
		BOOL rValue = FALSE;
		HANDLE hFind = ::FindFirstFile(strPath, &wfd);
		if ((hFind!=INVALID_HANDLE_VALUE) &&
			(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
		{
			rValue = TRUE;
		}
		FindClose(hFind);
		return rValue;
	}
	static BOOL CreateDirectory(CString strPath)
	{
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.bInheritHandle = FALSE;
		sa.lpSecurityDescriptor = NULL;
		return ::CreateDirectory(strPath, &sa);
	}
	static DWORD getFileSize(CString strFilePath)
	{
		WIN32_FIND_DATA wfd;
		DWORD fileSize = 0;
		HANDLE hFind = ::FindFirstFile(strFilePath, &wfd);
		if (hFind != INVALID_HANDLE_VALUE)
			fileSize = wfd.nFileSizeLow;
		FindClose(hFind);
		return fileSize;
	}
};