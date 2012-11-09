#include "StdAfx.h"
#include ".\filesearcher.h"

extern std::string WChar2Ansi(LPCWSTR pwszSrc);
extern std::wstring Ansi2WChar(LPCSTR pszSrc, int nLen);

namespace FileSearch
{
	HANDLE CFileSearcher::s_hSearchEvent = INVALID_HANDLE_VALUE;
	HANDLE CFileSearcher::s_hStopSearchEvent = INVALID_HANDLE_VALUE;
	string CFileSearcher::s_strSearch;
	unsigned long CFileSearcher::s_lSearchFileNum = 0;
	set<pair<string, string> > CFileSearcher::s_FilesSet;
	set<pair<string, string> > CFileSearcher::s_FilesSetResult;
	set<string> CFileSearcher::m_HisrotyFiles;		
	set<string> CFileSearcher::m_HistoryFolderes;	
	set<string> CFileSearcher::m_UserAddedFolderes;
	CRITICAL_SECTION CFileSearcher::m_cs;
	volatile bool CFileSearcher::s_bFinishIndexBuild = false;
	volatile bool CFileSearcher::s_bSearching = false;
	volatile bool CFileSearcher::s_bResultCopying = false;
	HANDLE CFileSearcher::m_hBuildIndex = INVALID_HANDLE_VALUE;
	HANDLE CFileSearcher::m_hSearchStartThread = INVALID_HANDLE_VALUE;
	HANDLE CFileSearcher::m_hSearchWorkerThread = INVALID_HANDLE_VALUE;


	CFileSearcher::CFileSearcher(void)
	{
		m_hSearchStartThread = INVALID_HANDLE_VALUE;
		m_hSearchWorkerThread = INVALID_HANDLE_VALUE;
		InitializeCriticalSection(&m_cs);
	}

	CFileSearcher::~CFileSearcher(void)
	{
		s_strSearch.clear();
		m_HisrotyFiles.clear();
		m_HistoryFolderes.clear();
		m_UserAddedFolderes.clear();
		s_FilesSet.clear();
		s_FilesSetResult.clear();
		CloseHandle(m_hSearchStartThread);
		CloseHandle(m_hSearchWorkerThread);
		CloseHandle(s_hSearchEvent);
		CloseHandle(s_hStopSearchEvent);
		OutputDebugString(_T("CFileSearcher析构!"));
	}

	bool CFileSearcher::start()
	{
		//if (s_FilesSet.empty())//
		//	return false;
		//启动搜索线程
		s_hSearchEvent = CreateEvent(NULL, TRUE, FALSE, _T("Searchevent"));
		s_hStopSearchEvent = CreateEvent(NULL, TRUE, TRUE, _T("Stopsearchevent"));//初始为激活状态，就是可以搜索
		if (s_hSearchEvent == INVALID_HANDLE_VALUE || s_hStopSearchEvent == INVALID_HANDLE_VALUE)
		{
			OutputDebugString(_T("创建事件时发生错误!"));
			return false;
		}
		m_hSearchStartThread = (HANDLE)_beginthreadex(NULL, 0, StartSearchThrd, NULL, 0, NULL);
		return true;
	}
	bool CFileSearcher::SearchStrChange(char* strSearch)
	{
		if (s_bSearching)//如果正在搜索，那么要中断当前的搜索
			SetSearchingState(false);
		else//没有在搜索的话，那么就直接激活停止搜索事件，准备继续搜索
			SetEvent(s_hStopSearchEvent);
		//在新的搜索开始前，要停止原来的搜索
		if (WAIT_OBJECT_0 != WaitForSingleObject(s_hStopSearchEvent, INFINITE))
		{
			OutputDebugString(_T("等待停止搜索时，出现错误!"));
			return false;
		}
		ResetEvent(s_hStopSearchEvent);//还原
		s_strSearch = strSearch;//先将字符串设置好，之后再启动搜索线程
		SetEvent(s_hSearchEvent);//激活事件，开始新的搜索
		return s_bSearching;
	}
	bool CFileSearcher::GetSearchResult(list<pair<string, string> >& result)
	{
		result.clear();
		s_bResultCopying = true;//并告知将要拷贝数据，插入下一条数据之后，就停止
		ResetEvent(s_hSearchEvent);//激活搜索线程
		EnterCriticalSection(&m_cs);
		for (set<pair<string, string> >::iterator iter = s_FilesSetResult.begin(); iter != s_FilesSetResult.end(); ++iter)
			result.push_back(*iter);
		LeaveCriticalSection(&m_cs);
		//还原
		s_bResultCopying = false;
		SetEvent(m_hSearchWorkerThread);
		//SetEvent(s_hResultCopyEvent);//拷贝完毕，激活陷入等待中的事件
		return !s_bSearching;
	}
	bool CFileSearcher::AddToFilesSet(pair<string, string>& p)
	{
		if (p.first.empty() || p.second.empty())
			return false;
		for (set<pair<string, string> >::iterator iter = s_FilesSet.begin(); iter != s_FilesSet.end(); ++iter)
		{
			if (iter->first.compare(p.first) == 0 && iter->second.compare(p.second) == 0)//重复
				return false;
		}
		s_FilesSet.insert(p);
		return true;
	}
	unsigned __stdcall CFileSearcher::BuildIndexThrd(void* pParam)
	{
		time_t TickCount = time(NULL);
		s_bFinishIndexBuild = false;
		FindSearchFolder();
		//Print();
		typedef pair<string, string> _sspair;
		string strRet;
		set<string>::iterator iter;
		set<string> spath;
		set<string>::iterator iter2;
		s_FilesSet.clear();//清空以前的记录
		for (iter = m_HisrotyFiles.begin(); iter != m_HisrotyFiles.end(); ++iter)
		{
			if (GetFileNameFromPath(*iter, strRet))
			{
				if (strRet != "")
					s_FilesSet.insert(_sspair(strRet, *iter));
			}
		}
		for (iter = m_HistoryFolderes.begin(); iter != m_HistoryFolderes.end(); ++iter)
		{
			spath.clear();
			GetAllDWGFileInPath(*iter, spath);//获得文件夹下所有文件路径
			for (iter2 = spath.begin(); iter2 != spath.end(); ++iter2)
			{
				if (GetFileNameFromPath(*iter2, strRet))
					s_FilesSet.insert(_sspair(strRet, *iter2));
			}
		}
		for (iter = m_UserAddedFolderes.begin(); iter != m_UserAddedFolderes.end(); ++iter)
		{
			spath.clear();
			GetAllDWGFileInPath(*iter, spath);//获得文件夹下所有文件路径
			for (iter2 = spath.begin(); iter2 != spath.end(); ++iter2)
			{
				if (GetFileNameFromPath(*iter2, strRet))
					s_FilesSet.insert(_sspair(strRet, *iter2));
			}
		}
		s_bFinishIndexBuild = true;
		TickCount = time(NULL) - TickCount;
		char count[50];
		sprintf(count, "搜索%d个文件，花费时间%d，得到dwg文件%d", s_lSearchFileNum, TickCount, s_FilesSet.size());
		OutputDebugString(count);
		return 0;
	}
	unsigned __stdcall CFileSearcher::StartSearchThrd(void* pParam)
	{
		while (1)//不断地准备搜索
		{
			if (WAIT_OBJECT_0 != WaitForSingleObject(s_hSearchEvent, INFINITE))
			{
				OutputDebugString(_T("搜索线程等待返回错误,线程退出!"));
				break;
			}
			ResetEvent(s_hSearchEvent);//重新设置事件为非激活状态
			//开始搜索
			worker();
		}
		return 0;
	}
	unsigned CFileSearcher::worker()
	{
		//OutputDebugString(_T("i'm working!\n"));
		//无论如何搜索，都要先清空
		EnterCriticalSection(&m_cs);
		s_FilesSetResult.clear();
		LeaveCriticalSection(&m_cs);
		if (s_FilesSet.empty() || s_strSearch.empty())
			return -1;
		SetSearchingState();//yes it's seraching something
		string strExtract, strSearch;
		size_t i = 0;
		for (i = 0; i < s_strSearch.length(); i++)
			strSearch += toupper(s_strSearch[i]);
		//OutputDebugString(("搜索 " + strSearch + "\n").c_str());
		for (set<pair<string, string> >::iterator iter = s_FilesSet.begin(); iter != s_FilesSet.end(); ++iter)
		{
			if (!CFileSearcher::s_bSearching)//若被设置为停止搜索
			{
				SetEvent(s_hStopSearchEvent);//表示已经停止搜索
				//OutputDebugString("\n本次搜索中断\n");
				break;
			}
			if (s_strSearch.length() > (*iter).first.length())
				continue;
			if (!IsResult(iter->first, strSearch))
				continue;
			EnterCriticalSection(&m_cs);
			s_FilesSetResult.insert(*iter);
			strExtract = (*iter).second;
			LeaveCriticalSection(&m_cs);
		}
		SetSearchingState(false);//searching finished
		return 0;
	}
	void CFileSearcher::FindSearchFolder()
	{
		//获得搜索文件夹
		GetACADHistoryFilesFolder();
		GetMiniCADSeeHistory();
		GetWindowsRecentHistory();
		m_HistoryFolderes.insert(m_UserAddedFolderes.begin(), m_UserAddedFolderes.end());
		GetDSeeHistory();
	}
	void CFileSearcher::GetACADHistoryFilesFolder()
	{
		string csRegPath = "Software\\Autodesk\\AutoCAD";
		HKEY hKey = NULL;
		LONG lnRes = RegOpenKeyEx(HKEY_CURRENT_USER, csRegPath.c_str(),0L,KEY_READ,&hKey);

		//////////////////////////////////////////////////////////////////////////
		//遍历子项
		HKEY hKeyItem = hKey;
		DWORD dwCountSubKey = 0;
		DWORD dwMaxSubkeyLen = 0;
		if (ERROR_SUCCESS != RegQueryInfoKey(hKeyItem, NULL, NULL, 0, &dwCountSubKey, &dwMaxSubkeyLen,
			NULL, NULL, NULL, NULL, NULL, NULL))
		{
			return;
		}
		if (dwCountSubKey >= 1)
		{
			TCHAR* szSubKeyName = new TCHAR[dwMaxSubkeyLen+1];
			HKEY hKeyVer = NULL;
			string strPath;
			for (DWORD index=0; index<dwCountSubKey; index++)//遍历AutoCAD
			{
				DWORD len = dwMaxSubkeyLen + 1;
				RegEnumKeyEx(hKeyItem, index, szSubKeyName,&len, 0, NULL, NULL, NULL);//得到版本

				TCHAR strRegPathVer[1024] = {0};
				wsprintf(strRegPathVer, "%s\\%s", csRegPath.c_str(), szSubKeyName);
				
				LONG lnResVer = RegOpenKeyEx(HKEY_CURRENT_USER, strRegPathVer, 0L, KEY_READ, &hKeyVer);//版本根项
				DWORD dwCount = 0;
				DWORD dwMax = 0;
				if (ERROR_SUCCESS == RegQueryInfoKey(hKeyVer, NULL, NULL, 0, &dwCount, &dwMax,
					NULL, NULL, NULL, NULL, NULL, NULL))
				{
					if (dwCount >= 1)
					{
						TCHAR* szKeyNameVer = new TCHAR[dwMax+1];
						for (DWORD nVer=0; nVer<dwCount; nVer++)
						{	
							DWORD lenver = dwMax + 1;
							RegEnumKeyEx(hKeyVer, nVer, szKeyNameVer, &lenver, 0, NULL, NULL, NULL);//得到子版本
							//////////////////////////////////////////////////////////////////////////
							TCHAR strRegVerItem[1024];
							wsprintf(strRegVerItem, "%s\\%s\\Recent File List", strRegPathVer, szKeyNameVer);
							HKEY hKeyDWG = NULL;
							LONG lnRes = RegOpenKeyEx(HKEY_CURRENT_USER, strRegVerItem,0L,KEY_READ,&hKeyDWG);
							if(lnRes == ERROR_SUCCESS)
							{
								DWORD dwType;
								BYTE strVersion[300]; DWORD dwLen = 300;
								for(int i=1; i<=100; i++)
								{
									ZeroMemory(strVersion,sizeof(strVersion));
									dwLen = 500;
									TCHAR csFileKey[1024];
									wsprintf(csFileKey, "File%d", i);
									lnRes = RegQueryValueEx(hKeyDWG,LPCTSTR(csFileKey), 0,&dwType,strVersion,(LPDWORD)&dwLen);
									if(lnRes != ERROR_SUCCESS) continue;
									strVersion[dwLen] = 0;	
									string tmp;
									for (DWORD i = 0; i < dwLen; ++i)
										tmp += strVersion[i];
									//if (!tmp.empty())//必须非空
									//	m_HisrotyFiles.insert(tmp);//?放开之后会在最后的搜索结果中重复
									if (GetFolderPathFromFilePath(tmp, strPath))//取得ACAD访问过的文件所在的文件夹
										m_HistoryFolderes.insert(strPath);
								}
							}
							if (hKeyDWG != NULL)
							{
								RegCloseKey(hKeyDWG);
							}
							//////////////////////////////////////////////////////////////////////////
						}					
						delete [] szKeyNameVer;
					}
				}
			}
			if (hKeyVer != NULL)
			{
				RegCloseKey(hKeyVer);
			}
			delete [] szSubKeyName;
		}
		//////////////////////////////////////////////////////////////////////////


		if(hKey != NULL)
		{
			RegCloseKey(hKey);
		}
	}
	void CFileSearcher::GetMiniCADSeeHistory()
	{
		string minifiles;
		if (!GetSpecialDir(minifiles))
			return ;
		if (*(minifiles.rbegin()) == '\\')
			minifiles.erase(minifiles.end() - 1);
		minifiles += "\\DSeeTem\\minifiles.txt";
		TCHAR buf[1024] = {0};
		ifstream ifs;
		ifs.open(minifiles.c_str(), ios::binary);
		string filepath;
		while (ifs.good())
		{
			ifs.getline(buf, sizeof(buf), '\n');
			filepath = buf;
			if (!filepath.empty())//非空才能插入数据
				m_HisrotyFiles.insert(filepath);
		}
	}
	void CFileSearcher::GetWindowsRecentHistory()
	{
		//判断操作系统版本，win7和vista上用户最近打开的文档的位置是不一样的
		bool bWin7orVista = false;
		OSVERSIONINFO oiex;
		ZeroMemory(&oiex, sizeof(OSVERSIONINFO));
		oiex.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (!GetVersionEx(&oiex))
			return ;
		switch (oiex.dwPlatformId)
		{
		case VER_PLATFORM_WIN32_NT:
			if (oiex.dwMajorVersion == 6/* && oiex.dwMinorVersion == 0/1*/)
				bWin7orVista = true;
			break;
		case VER_PLATFORM_WIN32_WINDOWS:
			//winodws 2000 以下的版本
			bWin7orVista = false;
			break;
		}
		vector<string> paths;
		GetRecentFilePath(bWin7orVista, paths);
		GetPathFromShellLink(paths);
	}
	bool CFileSearcher::GetRecentFilePath(bool bWin7OrVista, vector<string>& paths)
	{
		string path, fixedpathstr;
		vector<string> childpaths;
		if (bWin7OrVista)
		{
			TCHAR syssector[10];
			GetSystemDirectory(syssector, sizeof(syssector));
			GetWindowsDirectory(syssector, sizeof(syssector));
			path = syssector;
			path += "\\Users\\";
			vector<string> chiledpaths;
			GetChildrenFolder(path, childpaths);
			fixedpathstr = "\\AppData\\Roaming\\Microsoft\\Windows\\Recent\\";
		}
		else
		{
			GetSpecialDir(path, CSIDL_RECENT);//
			size_t pos = path.rfind('\\');
			path = path.substr(0, pos);
			pos = path.rfind('\\');
			path = path.substr(0, pos);
			GetChildrenFolder(path, childpaths);
			fixedpathstr = "\\Recent\\";
		}
		for (vector<string>::iterator iter = childpaths.begin(); iter != childpaths.end(); ++iter)
		{
			if (PathFileExists((*iter + fixedpathstr).c_str()))//路径是否存在
				paths.push_back(*iter + fixedpathstr);
		}
		return true;
	}
	bool CFileSearcher::GetChildrenFolder(string path, vector<string>& chiledpaths)
	{
		if (path.empty())
			return false;
		if (path[path.size() - 1] != '\\')
			path += '\\';
		//path += "*.*";
		//OutputDebugString((path + '\n').c_str());
		WIN32_FIND_DATA FindData;
		HANDLE hFind = ::FindFirstFile((path + "*.*").c_str(), &FindData);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			path.clear();
			return false;
		}
		while (1)
		{
			if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (FindData.cFileName[0] != '.')
					chiledpaths.push_back(path + FindData.cFileName);
			}
			if (!FindNextFile(hFind, &FindData))
				break;
		}
		return true;
	}
	void CFileSearcher::GetDSeeHistory()
	{
		string path;
		if (!GetSpecialDir(path))
			return ;
		path += "\\DSeeTem\\downloadfile";
		set<string> filepaths;
		GetAllDWGFileInPath(path, filepaths);
		for (set<string>::iterator iter = filepaths.begin(); iter != filepaths.end(); ++iter)
			m_HisrotyFiles.insert(*iter);
	}
	bool CFileSearcher::IsResult(string strSource, string strTarget)
	{
		////例如：两1个2月s --> strSource    L1G2Ys --> strTarget   就会返回true
		if (strTarget.empty() || strTarget.empty())
			return false;
		typedef unsigned char UBYTE;
		static int iSource = 0, iTarget = 0;
		iSource = 0; iTarget = 0;
		static size_t indexs = 0, indext = 0;
		indexs = 0; indext = 0;
		//static bool bCurTarget = true;//记录当前的目标字符是否是ascii码的前128个字符
		//检查剩余字符数是否满足Rest[strTarget] < Rest[strSource]
		while ((strSource.size() - indexs >= strTarget.size() - indext) && strSource.size() > indexs)
		{
			//为iSource, iTarget赋值，准备下一次匹配的字符的值
			if ((UBYTE)strTarget[indext] < 0x80)
			{
				iTarget = (UBYTE)strTarget[indext];
				//bCurTarget = true;
			}
			else
			{
				if ((UBYTE)strTarget[indext] < 0xA0)//不是汉字，不是ascii码的128个字符中的一个，那就是奇怪的字符
					return false;
				//bCurTarget = false;
				iTarget = ((UBYTE)strTarget[indext] - 0xA0) * 100 + (UBYTE)strTarget[indext + 1] - 0xA0;
			}
			if ((UBYTE)strSource[indexs] < 0x80)
				iSource = (UBYTE)strSource[indexs];
			else
			{
				if ((UBYTE)strSource[indexs] < 0xA0)//不是汉字，不是ascii码的128个字符中的一个
					return false;
				iSource = ((UBYTE)strSource[indexs] - 0xA0) * 100 + (UBYTE)strSource[indexs + 1] - 0xA0;
			}

			if (iTarget < 0x80)//匹配目标串为128个ascii码中的一个
			{
				if (iSource < 0x80)//字母数字
					indexs++;
				else//汉字
				{
					indexs += 2;
					iSource = GetCharacterFirstLetter(iSource);
				}
				if (toupper(iSource) == toupper(iTarget))
					indext++;
				//不匹配的话，indext不动
			}
			else//匹配目标为汉字，必须原值匹配，不做转化
			{
				if (iSource < 0xA0)//源字符不是汉字，不必作对比，肯定不相同
				{
					indexs++;
					continue;
				}
				if (toupper(iSource) == toupper(iTarget))
				{
					indext += 2;
					indexs += 2;
				}
				else
					indexs += 2;
			}
		}
		if (indext >= strTarget.size())//目标字符串已经对比完了
			return true;
		else
			return false;
	}
	char CFileSearcher::GetCharacterFirstLetter(int nCode)
	{
		if(nCode >= 1601 && nCode < 1637) return  'A'; 
		if(nCode >= 1637 && nCode < 1833) return  'B';
		if(nCode >= 1833 && nCode < 2078) return  'C';
		if(nCode >= 2078 && nCode < 2274) return  'D';
		if(nCode >= 2274 && nCode < 2302) return  'E';
		if(nCode >= 2302 && nCode < 2433) return  'F';
		if(nCode >= 2433 && nCode < 2594) return  'G';
		if(nCode >= 2594 && nCode < 2787) return  'H';
		if(nCode >= 2787 && nCode < 3106) return  'J';
		if(nCode >= 3106 && nCode < 3212) return  'K';
		if(nCode >= 3212 && nCode < 3472) return  'L';
		if(nCode >= 3472 && nCode < 3635) return  'M';
		if(nCode >= 3635 && nCode < 3722) return  'N';
		if(nCode >= 3722 && nCode < 3730) return  'O';
		if(nCode >= 3730 && nCode < 3858) return  'P';
		if(nCode >= 3858 && nCode < 4027) return  'Q';
		if(nCode >= 4027 && nCode < 4086) return  'R';
		if(nCode >= 4086 && nCode < 4390) return  'S';
		if(nCode >= 4390 && nCode < 4558) return  'T';
		if(nCode >= 4558 && nCode < 4684) return  'W';
		if(nCode >= 4684 && nCode < 4925) return  'X';
		if(nCode >= 4925 && nCode < 5249) return  'Y'; 
		if(nCode >= 5249 && nCode < 5590) return  'Z';
		return '\0';
	}
	bool CFileSearcher::BuildIndex()
	{
		m_hBuildIndex = (HANDLE)_beginthreadex(NULL, 0, BuildIndexThrd, NULL, 0, NULL);
		return true;
	}
	bool CFileSearcher::AddUserFolder(string strFolder)
	{
		DWORD attr = GetFileAttributes(strFolder.c_str());
		if (attr == INVALID_FILE_ATTRIBUTES || !(attr & FILE_ATTRIBUTE_DIRECTORY))
			return false;
		pair<set<string>::iterator, bool> p;
		p = m_UserAddedFolderes.insert(strFolder);
		return p.second;
	}
	bool CFileSearcher::GetSpecialDir(string& mydoc, int csidl)
	{
		mydoc.clear();
		char szDocument[MAX_PATH];
		LPITEMIDLIST pidl;
		SHGetSpecialFolderLocation(NULL, csidl, &pidl);
		if (pidl && SHGetPathFromIDList(pidl, szDocument))
		{
			mydoc.assign(szDocument);
			return true;
		}
		return false;
	}
	bool CFileSearcher::GetPathFromShellLink(vector<string>& linkfilepath)
	{
		::CoInitialize(0);
		HRESULT hRet;
		IShellLink *plink;
		IPersistFile* ppf;
		hRet = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void **)&plink);
		if (hRet != S_OK)
		{
			//failed
			::CoUninitialize();
			return false;
		}
		else
		{
			hRet = plink->QueryInterface(IID_IPersistFile, (void**)&ppf);
			if (hRet != S_OK)
			{
				return false;
			}
			else//获取接口成功
			{
				//遍历文件夹，逐个检查快捷方式
				string curfile;
				wstring wcurfile;
				TCHAR filepath[MAX_PATH];
				WIN32_FIND_DATA FindData;
				size_t index = 0;
				for (vector<string>::iterator iter = linkfilepath.begin(); iter != linkfilepath.end(); ++iter)
				{
					curfile = *iter + "*.*";
					HANDLE hFile = FindFirstFile(curfile.c_str(), &FindData);
					while (1)
					{
						if (FindData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)//如果是快捷方式文件的话
						{
							wcurfile = Ansi2WChar((*iter + FindData.cFileName).c_str(), (int)(*iter + FindData.cFileName).size());
							if (ppf->Load(wcurfile.c_str(), STGM_READ) == S_OK)
							{
								memset(filepath, 0, sizeof(filepath));
								plink->GetPath(filepath, MAX_PATH, NULL, 0);
								if (GetFileAttributes(filepath) == FILE_ATTRIBUTE_DIRECTORY)
								{
									if (m_HistoryFolderes.count(filepath) == 0)
										m_HistoryFolderes.insert(filepath);
								}
								else
								{
									curfile = filepath;
									index = curfile.length();
									if ((curfile[index - 3] == 'D' || curfile[index - 3] == 'd') && 
										(curfile[index - 2] == 'W' || curfile[index - 2] == 'w') && 
										(curfile[index - 1] == 'G' || curfile[index - 1] == 'g'))
									{
										m_HisrotyFiles.insert(filepath);
									}
								}
							}
						}
						if (!FindNextFile(hFile, &FindData))
							break;
					}
				}
			}
		}
		return true;
	}

	bool CFileSearcher::GetAllDWGFileInPath(string path, set<string>& filepaths)
	{
		if (!bSearchAllFolder)//默认不搜索全盘
		{
			if (path[path.size() - 1] != '\\')
				path += '\\';
			string mypath = path;
			//只找dwg文件
			mypath += "*.dwg";//用dwg后缀来过滤搜索对象
			WIN32_FIND_DATA FindData;
			HANDLE hFind = ::FindFirstFile(mypath.c_str(), &FindData);
			if (hFind == INVALID_HANDLE_VALUE)
				return false;
			while (1)
			{
				if (FindData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
					filepaths.insert(path + FindData.cFileName);
				if (!FindNextFile(hFind, &FindData))
					break;
			}
			//只找文件夹
			mypath = path + "*.*";
			hFind = ::FindFirstFile(mypath.c_str(), &FindData);
			if (hFind == INVALID_HANDLE_VALUE)
				return false;
			while (1)
			{
				s_lSearchFileNum++;
				if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					if (FindData.cFileName[0] != '.')
						GetAllDWGFileInPath(path + FindData.cFileName, filepaths);
				}
				if (!FindNextFile(hFind, &FindData))
					break;
			}
			return true;
		}
		else
		{
			//全盘搜索，而后过滤出dwg文件
			if (path[path.size() - 1] != '\\')
				path += '\\';
			string mypath = path;
			//只找dwg文件
			mypath += "*.*";
			string strExt;
			WIN32_FIND_DATA FindData;
			HANDLE hFind = ::FindFirstFile(mypath.c_str(), &FindData);
			if (hFind == INVALID_HANDLE_VALUE)
				return false;
			while (1)
			{
				if (FindData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
				{
					s_lSearchFileNum++;
					strExt = FindData.cFileName;
					if (strExt.length() > 3)
					{
						if (strExt.substr(strExt.length() - 3).compare("dwg") == 0)
							filepaths.insert(path + FindData.cFileName);
					}
				}
				else if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					if (FindData.cFileName[0] != '.')
						GetAllDWGFileInPath(path + FindData.cFileName, filepaths);
				}
				if (!FindNextFile(hFind, &FindData))
					break;
			}
			return true;
		}
	}

	bool CFileSearcher::GetFileNameFromPath(string strPath, string& strFileName)
	{
		strFileName.clear();
		if (strPath.empty())
			return false;
		DWORD attr = GetFileAttributes(strPath.c_str());
		s_lSearchFileNum++;
		if (attr == INVALID_FILE_ATTRIBUTES || attr & FILE_ATTRIBUTE_DIRECTORY)//错误和文件夹路径均不作处理
		{
			//OutputDebugString(("已经不存在的路径" + strPath).c_str());
			return false;
		}
		static size_t pos = string::npos;
		pos = strPath.find_last_of('\\');
		if (pos == string::npos)
			return false;
		strFileName = strPath.substr(pos + 1);
		return true;
	}
	bool CFileSearcher::GetFolderPathFromFilePath(string strFilePath, string& strFolderPath)
	{
		if (strFilePath.empty())
			return false;
		strFolderPath.clear();
		DWORD attr = GetFileAttributes(strFilePath.c_str());
		if (attr == INVALID_FILE_ATTRIBUTES || attr & FILE_ATTRIBUTE_DIRECTORY)//错误和文件夹路径均不作处理
			return false;
		string::size_type pos = string::npos;
		if ((pos = strFilePath.find_last_of('\\')) == string::npos)
			return false;
		strFolderPath = strFilePath.substr(0, pos);
		attr = GetFileAttributes(strFolderPath.c_str());
		if (attr == 22)//盘符
			return true;
		if (attr == INVALID_FILE_ATTRIBUTES || attr != FILE_ATTRIBUTE_DIRECTORY)//错误或者不是文件夹，也不是盘符路径
		{
			//if (PathIsRoot(strFolderPath.c_str()))
			//	return true;
			strFolderPath.clear();
			return false;
		}
		if (strFolderPath.empty())//为空也返回false
			return false;
		//strFolderPath.assign(strPath.c_str(), pos + 1);
		//OutputDebugString(("文件夹路径\t" + strFolderPath).c_str());
		return true;
	}
}