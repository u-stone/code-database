#pragma once

namespace FileSearch
{
	static bool bSearchAllFolder = true;//是否搜索部指定的文件夹
	class CFileSearcher
	{
	public:
		CFileSearcher(void);
		virtual ~CFileSearcher(void);
	private:
		static HANDLE s_hSearchEvent;			//线程搜索控制事件
		static HANDLE s_hStopSearchEvent;		//停止搜索到饿事件
		static HANDLE m_hBuildIndex;			//记录建立索引的线程句柄
		static HANDLE m_hSearchStartThread;		//搜索启动线程的句柄
		static HANDLE m_hSearchWorkerThread;	//搜索工作线程的句柄
		static string s_strSearch;				//记录要搜索的字符串
		static unsigned long s_lSearchFileNum;	//记录搜索过的文件的数量
		//使用set<pair<string, string> >是为了解决不同文件夹下文件同名的问题，而同时又要把不同文件夹下同名的列出来，这个成员只做一次索引建立
		static set<pair<string, string> > s_FilesSet;	//搜索内容的索引<文件名，文件路径>，
		static set<pair<string, string> > s_FilesSetResult;//搜索得到的结果
		static volatile bool s_bFinishIndexBuild;		//指示是否建立好索引
		static volatile bool s_bSearching;		//停止当前的搜索
		static volatile bool s_bResultCopying;	//指示是否正在拷贝数据
		static CRITICAL_SECTION m_cs;			//保护搜索结果数据s
		static set<string> m_HisrotyFiles;		//历史文件
		static set<string> m_HistoryFolderes;	//历史文件夹
		static set<string> m_UserAddedFolderes;	//用户添加的搜索文件夹
	public:
		bool start();
		static void SetSearchStr(string str) {
			s_strSearch.assign(str);
		}
		static bool GetSearchResult(list<pair<string, string> >& result);//返回true表示取数据时搜索已经结束，返回false表示取数据时搜索未完成
		bool BuildIndex();//建立索引
		bool AddUserFolder(string strFolder);
		static bool SearchStrChange(char* strSearch);//搜索的内容改变了，要调用的函数，开始新的搜索
	private:
		static void SetSearchingState(bool bsearching = true){
			s_bSearching = bsearching;
		}
		bool AddToFilesSet(pair<string, string>& p);
	private:
		//开启搜索work线程
		static unsigned __stdcall BuildIndexThrd(void* pParam);
		static unsigned __stdcall StartSearchThrd(void* pParam);
		static unsigned worker();//搜索工作的执行者

	private:
		static void FindSearchFolder();//找到所有需要搜索的文件夹
		//搜索顺序是：1. ACAD历史文件; 2. 迷你看图历史文件; 3. Windows历史目录; 4. 用户自己添加的目录; 5. 看图说话接收文件的目录
		static void GetACADHistoryFilesFolder();
		static void GetMiniCADSeeHistory();
		static void GetWindowsRecentHistory();
		static void GetDSeeHistory();
	private:
		//判断是否可以作为搜索结果:strSource是作为文件字符集结果中的一条记录，strTarget作为搜索关键字符
		static bool IsResult(string strSource, string strTarget);
		static char GetCharacterFirstLetter(int nCode);

	private:
		static bool GetSpecialDir(string& mydoc, int csidl = CSIDL_PERSONAL);//获得特定文件路径
		static bool GetDSFolderDir(string& strDs);//获得Documents and Settings文件夹的路径
		static bool GetRecentFilePath(bool bWin7OrVista, vector<string>& paths);//获得最近打开文件的路径，vista,win7与之前版本的操作系统有所不同
		static bool GetPathFromShellLink(vector<string>& linkfilepath);//从快捷方式中读取出指向的文件全路径
		static bool GetChildrenFolder(string path, vector<string>& chiledpaths);//遍历子级目录，返回所有文件夹
		static bool GetAllDWGFileInPath(string path, set<string>& filepaths);//遍历指定的路径，返回所有子文件夹的文件
		static bool GetFileNameFromPath(string strPath, string& strFileName);//从路径中解析出文件名
		static bool GetFolderPathFromFilePath(string strPath, string& strFolderPath);//从文件目录中得到文件所在文件夹的路径
		//将字符由小写转为大写
		static int toupper(int c) {
			if (c >= 'a' && c <= 'z')
				return c - 32;
			return c;
		}
		//void Print()
		//{
		//	OutputDebugString("==================================================================\n");
		//	for (set<string>::iterator iter = m_HisrotyFiles.begin(); iter != m_HisrotyFiles.end(); ++iter)
		//		OutputDebugString(("历史文件：" + *iter).c_str());
		//	for (set<string>::iterator iter = m_HistoryFolderes.begin(); iter != m_HistoryFolderes.end(); ++iter)
		//		OutputDebugString(("文件夹：" + *iter).c_str());
		//	for (set<string>::iterator iter = m_UserAddedFolderes.begin(); iter != m_UserAddedFolderes.end(); ++iter)
		//		OutputDebugString(("用户添加文件夹" + *iter).c_str());
		//	OutputDebugString("==================================================================\n");
		//}
		//void PrintFilesMap()
		//{
		//	OutputDebugString("==================================================================\n");
		//	string cmd;
		//	int i = 0;
		//	CString str;
		//	for (set<pair<string, string> >::iterator iter = s_FilesSet.begin(); iter != s_FilesSet.end(); ++iter)
		//	{
		//		str.Format(_T("%d:\t"), ++i);
		//		OutputDebugString(str);
		//		cmd = "[ " + (*iter).first+ " ]---->  " + (*iter).second;
		//		OutputDebugString(cmd.c_str());
		//		OutputDebugString("\n");
		//	}
		//	OutputDebugString("==================================================================\n");
		//}
	};
}
