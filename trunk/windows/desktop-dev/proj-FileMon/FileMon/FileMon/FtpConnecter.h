#pragma once
#include <afxinet.h>
#include <vector>
using namespace std;

#define TASKPERIOD		24*60*60	//����һ���ʱ�� ��λ����
//brief:
//�������Ҫ���������������FTP���Լ���ʱ��FTP�������ϴ��ļ�����ȻҲҪά��FTP������Ϣ

//brief:
//����ṹ�洢FTP������Ϣ
struct FtpInfo 
{
	FtpInfo(){
		_hWaitableTimer = INVALID_HANDLE_VALUE;
		_nFtpServerPort = 21;//FTPĬ�϶˿�
		_strFtpServerIp = _T("");
		_strUserName = _T("");
		_strUserPw = _T("");
		_strFolderPath = _T("");
	}
	HANDLE	_hWaitableTimer;
	int		_nFtpServerPort;
	CString _strFtpServerIp;
	CString _strUserName;
	CString _strUserPw;
	CString _strFolderPath;
};

class FtpConnecter
{
public:
	FtpConnecter(void);
	~FtpConnecter(void);
public:
	//�����ϴ��ļ����̣߳��߳��ϴ����ļ�֮����˳�
	static DWORD WINAPI UploadeFile(LPVOID param);
	//ֹͣFTP�ϴ�����
	static void stopUploadTask();
	//���ñ���Ҫ�ϴ��ļ��ĸ�Ŀ¼
	static void setLocalRootPath(CString strLocalRootPath);
	//����FTP�������ϴ���ϴ��ļ��ĸ�Ŀ¼
	static void setRemoteRootPath(CString strRemoteRootPath);
	//д�������Ϣ����������Ὣm_bUpdate����Ϊtrue
	static void pushTrackInfo(LPCTSTR info);

	//����FTP��Ϣ�����FTP��Ϣû��������ô�ͷ���TRUE�����򷵻�FALSE
	void setFtpinfo(CString& _strFtpServerIp, CString& _strUserName, CString& _strUserPw, int _nFtpServerPort);
	//���ÿ�ʼ�ϴ���ʱ�䣬���Ҫת��ΪFILETIME��ת��ΪUTCʱ��
	void setBeginTime(SYSTEMTIME& beginTimeSt);
	//�����ϴ��ļ�����ִ�е�ʱ����������ĵ�λ��1s��
	void setUploadTimePeriod(unsigned long lPeroid);
	//���ú���ز���(�ϴ��ļ���FTP��Ϣ���ϴ��ļ���ʱ������)֮�󣬿�ʼִ�����õ�����
	BOOL startUploadTask();
	//�������õ�FTP��Ϣ�Ƿ����������FTP�����������ӵ��Ͼͷ���true�����򷵻�false
	BOOL tryConnectFtp();
	//�õ����ص�ǰĿ¼
	CString getCurLocalFolderPath();
	//�õ���ǰFTP�������ϵ�Ŀ��·��
	CString getCurRemoteFolderPath();
	//�õ���ǰ�߳��Ƿ�������
	static BOOL getThreadRunning();
	//�õ�ÿ���ϴ��ļ���ʱ��
	CString getUploadBeginTime();

private:
	//��ʼ����ز���
	void initTimeInfo();
	//�õ������ļ���·��
	static CString getLocalFolderPath();
	//�õ�FTP�������ϵ��ļ�·��
	static CString getRemoteFolderPath();
	//�õ�������Ҫ�ϴ���ȫ���ļ�
	static BOOL getAllFilesUpload(std::vector<CString>& fiels);
	//�õ�ǰһ���ʱ��
	static void getPreviousDate(CString& strYear, CString& strMonth, CString& strDay, CString& strHour, CString& strMinutes);
	//�õ����ڵ�ʱ��
	static void getCurrentDate(CString& strYear, CString& strMonth, CString& strDay, CString& strHour, CString& strMinutes);
	//�õ������Ƿ�Ӧ���ϴ�
	static bool doUpload();
private:
	static BOOL	s_bRetryUpload;			//!< ��¼�Ƿ����ٴ��ϴ��ϴ��ϴ�ʧ�ܵ��ļ�����������ֻ����һ��
	static BOOL	s_bHasUploadFailedFile;	//!< ��¼�Ƿ����ϴ�ʧ�ܵ��ļ�
	static BOOL	s_bStopTask;			//!< ִֹͣ������
	static BOOL s_bThreadRunning;		//!< ��¼��ǰ�߳��Ƿ�������
	HANDLE		m_hWorkThread;			//!< �ϴ��ļ��̵߳ľ��
	long		m_lTimePeriod;			//!< �ϴ�ʱ��ļ��
	LARGE_INTEGER m_liBeginTime;		//!< �ϴ�ʱ��ľ��Կ�ʼʱ�䣬���ֵ��һ��UTCʱ�����ö���
	FILETIME	m_BeginTimeFT;			//!< ��ʼ�ϴ��ļ���UTC(Э������ʱ)ʱ�䣬���ֵ��m_BeginTimeSTת������
	FtpInfo		m_FtpInfo;				//!< FTP����������Ϣ
	static CString		s_strLocalRootPath;		//!< �����ϴ��ļ��ĸ�Ŀ¼
	static CString		s_strRemoteRootPath;		//!< FTP�������ϴ���ϴ��ļ��ĸ�Ŀ¼
	static std::vector<CString> s_FileFailedUpload;	//!< �ϴ�ʧ�ܵ��ļ�
};