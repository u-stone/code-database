#pragma once
#include <afxinet.h>
#include <vector>
using namespace std;

#define TASKPERIOD		24*60*60	//这是一天的时间 单位是秒
//brief:
//这个类主要用来处理测试连接FTP，以及定时向FTP服务器上传文件，当然也要维护FTP连接信息

//brief:
//这个结构存储FTP连接信息
struct FtpInfo 
{
	FtpInfo(){
		_hWaitableTimer = INVALID_HANDLE_VALUE;
		_nFtpServerPort = 21;//FTP默认端口
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
	//启动上传文件的线程，线程上传完文件之后就退出
	static DWORD WINAPI UploadeFile(LPVOID param);
	//停止FTP上传任务
	static void stopUploadTask();
	//设置本地要上传文件的根目录
	static void setLocalRootPath(CString strLocalRootPath);
	//设置FTP服务器上存放上传文件的根目录
	static void setRemoteRootPath(CString strRemoteRootPath);
	//写入跟踪信息，这个函数会将m_bUpdate设置为true
	static void pushTrackInfo(LPCTSTR info);

	//设置FTP信息，如果FTP信息没有问题那么就返回TRUE，否则返回FALSE
	void setFtpinfo(CString& _strFtpServerIp, CString& _strUserName, CString& _strUserPw, int _nFtpServerPort);
	//设置开始上传的时间，这个要转换为FILETIME并转换为UTC时间
	void setBeginTime(SYSTEMTIME& beginTimeSt);
	//设置上传文件任务执行的时间间隔，这里的单位是1s，
	void setUploadTimePeriod(unsigned long lPeroid);
	//设置好相关参数(上传文件的FTP信息，上传文件的时间设置)之后，开始执行设置的任务
	BOOL startUploadTask();
	//测试设置的FTP信息是否可以连的上FTP服务器，连接的上就返回true，否则返回false
	BOOL tryConnectFtp();
	//得到本地当前目录
	CString getCurLocalFolderPath();
	//得到当前FTP服务器上的目标路径
	CString getCurRemoteFolderPath();
	//得到当前线程是否在运行
	static BOOL getThreadRunning();
	//得到每天上传文件的时间
	CString getUploadBeginTime();

private:
	//初始化相关参数
	void initTimeInfo();
	//得到本地文件的路径
	static CString getLocalFolderPath();
	//得到FTP服务器上的文件路径
	static CString getRemoteFolderPath();
	//得到本地需要上传的全部文件
	static BOOL getAllFilesUpload(std::vector<CString>& fiels);
	//得到前一天的时间
	static void getPreviousDate(CString& strYear, CString& strMonth, CString& strDay, CString& strHour, CString& strMinutes);
	//得到现在的时间
	static void getCurrentDate(CString& strYear, CString& strMonth, CString& strDay, CString& strHour, CString& strMinutes);
	//得到现在是否应该上传
	static bool doUpload();
private:
	static BOOL	s_bRetryUpload;			//!< 记录是否尝试再次上传上次上传失败的文件，不过这里只重试一次
	static BOOL	s_bHasUploadFailedFile;	//!< 记录是否有上传失败的文件
	static BOOL	s_bStopTask;			//!< 停止执行任务
	static BOOL s_bThreadRunning;		//!< 记录当前线程是否在运行
	HANDLE		m_hWorkThread;			//!< 上传文件线程的句柄
	long		m_lTimePeriod;			//!< 上传时间的间隔
	LARGE_INTEGER m_liBeginTime;		//!< 上传时间的绝对开始时间，这个值由一个UTC时间设置而来
	FILETIME	m_BeginTimeFT;			//!< 开始上传文件的UTC(协调世界时)时间，这个值由m_BeginTimeST转换而来
	FtpInfo		m_FtpInfo;				//!< FTP服务器的信息
	static CString		s_strLocalRootPath;		//!< 本地上传文件的根目录
	static CString		s_strRemoteRootPath;		//!< FTP服务器上存放上传文件的根目录
	static std::vector<CString> s_FileFailedUpload;	//!< 上传失败的文件
};