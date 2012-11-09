#pragma once

namespace FileSearch
{
	static bool bSearchAllFolder = true;//�Ƿ�������ָ�����ļ���
	class CFileSearcher
	{
	public:
		CFileSearcher(void);
		virtual ~CFileSearcher(void);
	private:
		static HANDLE s_hSearchEvent;			//�߳����������¼�
		static HANDLE s_hStopSearchEvent;		//ֹͣ���������¼�
		static HANDLE m_hBuildIndex;			//��¼�����������߳̾��
		static HANDLE m_hSearchStartThread;		//���������̵߳ľ��
		static HANDLE m_hSearchWorkerThread;	//���������̵߳ľ��
		static string s_strSearch;				//��¼Ҫ�������ַ���
		static unsigned long s_lSearchFileNum;	//��¼���������ļ�������
		//ʹ��set<pair<string, string> >��Ϊ�˽����ͬ�ļ������ļ�ͬ�������⣬��ͬʱ��Ҫ�Ѳ�ͬ�ļ�����ͬ�����г����������Աֻ��һ����������
		static set<pair<string, string> > s_FilesSet;	//�������ݵ�����<�ļ������ļ�·��>��
		static set<pair<string, string> > s_FilesSetResult;//�����õ��Ľ��
		static volatile bool s_bFinishIndexBuild;		//ָʾ�Ƿ���������
		static volatile bool s_bSearching;		//ֹͣ��ǰ������
		static volatile bool s_bResultCopying;	//ָʾ�Ƿ����ڿ�������
		static CRITICAL_SECTION m_cs;			//���������������s
		static set<string> m_HisrotyFiles;		//��ʷ�ļ�
		static set<string> m_HistoryFolderes;	//��ʷ�ļ���
		static set<string> m_UserAddedFolderes;	//�û���ӵ������ļ���
	public:
		bool start();
		static void SetSearchStr(string str) {
			s_strSearch.assign(str);
		}
		static bool GetSearchResult(list<pair<string, string> >& result);//����true��ʾȡ����ʱ�����Ѿ�����������false��ʾȡ����ʱ����δ���
		bool BuildIndex();//��������
		bool AddUserFolder(string strFolder);
		static bool SearchStrChange(char* strSearch);//���������ݸı��ˣ�Ҫ���õĺ�������ʼ�µ�����
	private:
		static void SetSearchingState(bool bsearching = true){
			s_bSearching = bsearching;
		}
		bool AddToFilesSet(pair<string, string>& p);
	private:
		//��������work�߳�
		static unsigned __stdcall BuildIndexThrd(void* pParam);
		static unsigned __stdcall StartSearchThrd(void* pParam);
		static unsigned worker();//����������ִ����

	private:
		static void FindSearchFolder();//�ҵ�������Ҫ�������ļ���
		//����˳���ǣ�1. ACAD��ʷ�ļ�; 2. ���㿴ͼ��ʷ�ļ�; 3. Windows��ʷĿ¼; 4. �û��Լ���ӵ�Ŀ¼; 5. ��ͼ˵�������ļ���Ŀ¼
		static void GetACADHistoryFilesFolder();
		static void GetMiniCADSeeHistory();
		static void GetWindowsRecentHistory();
		static void GetDSeeHistory();
	private:
		//�ж��Ƿ������Ϊ�������:strSource����Ϊ�ļ��ַ�������е�һ����¼��strTarget��Ϊ�����ؼ��ַ�
		static bool IsResult(string strSource, string strTarget);
		static char GetCharacterFirstLetter(int nCode);

	private:
		static bool GetSpecialDir(string& mydoc, int csidl = CSIDL_PERSONAL);//����ض��ļ�·��
		static bool GetDSFolderDir(string& strDs);//���Documents and Settings�ļ��е�·��
		static bool GetRecentFilePath(bool bWin7OrVista, vector<string>& paths);//���������ļ���·����vista,win7��֮ǰ�汾�Ĳ���ϵͳ������ͬ
		static bool GetPathFromShellLink(vector<string>& linkfilepath);//�ӿ�ݷ�ʽ�ж�ȡ��ָ����ļ�ȫ·��
		static bool GetChildrenFolder(string path, vector<string>& chiledpaths);//�����Ӽ�Ŀ¼�����������ļ���
		static bool GetAllDWGFileInPath(string path, set<string>& filepaths);//����ָ����·���������������ļ��е��ļ�
		static bool GetFileNameFromPath(string strPath, string& strFileName);//��·���н������ļ���
		static bool GetFolderPathFromFilePath(string strPath, string& strFolderPath);//���ļ�Ŀ¼�еõ��ļ������ļ��е�·��
		//���ַ���СдתΪ��д
		static int toupper(int c) {
			if (c >= 'a' && c <= 'z')
				return c - 32;
			return c;
		}
		//void Print()
		//{
		//	OutputDebugString("==================================================================\n");
		//	for (set<string>::iterator iter = m_HisrotyFiles.begin(); iter != m_HisrotyFiles.end(); ++iter)
		//		OutputDebugString(("��ʷ�ļ���" + *iter).c_str());
		//	for (set<string>::iterator iter = m_HistoryFolderes.begin(); iter != m_HistoryFolderes.end(); ++iter)
		//		OutputDebugString(("�ļ��У�" + *iter).c_str());
		//	for (set<string>::iterator iter = m_UserAddedFolderes.begin(); iter != m_UserAddedFolderes.end(); ++iter)
		//		OutputDebugString(("�û�����ļ���" + *iter).c_str());
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
