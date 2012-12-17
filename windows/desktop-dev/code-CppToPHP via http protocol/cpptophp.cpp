void PostResult(CString strFileName, bool bSuc)
{
	static CInternetSession sess;//建立会话 
	static CString strUrl = _T("");
	INTERNET_PORT nPort = 80;
	CHttpConnection* pServer = sess.GetHttpConnection(_T("192.168.3.104"), nPort);
	CHttpFile *pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/test.php"));

	CString strHeaders = _T("Content-Type: application/x-www-form-urlencoded"); 
	CString strData;
	strData.Format(_T("FilePath=%s&Result=%s&action=submit"), strFileName, bSuc ? _T("1") : _T("0"));
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
	sess.Close();
}
int main()
{
	PostResult(_T("PostHttpPage succeded"), true);
}