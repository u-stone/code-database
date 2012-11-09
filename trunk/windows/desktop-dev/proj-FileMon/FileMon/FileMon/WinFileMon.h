#pragma once
#include <set>
#include <vector>
using namespace std;

struct MonData;

class ProcTracker;

//!WinFileMon类
/*!
	该类是让用户添加或者删除监视路径的。并启动和停止响应的监视线程.
*/
class WinFileMon
{
public:
	WinFileMon(void);
	~WinFileMon(void);
public:
	//Summary:
	//	添加要监视的文件夹strDir，并启动线程开始监视
	bool addMonitorDir(CString strDir);
	//Summary:
	//	删除要监视的文件夹strDir
	void removeMonitorDir(CString strDir);
private:
	//Summary:	启动线程执行新的文件夹监视
	//Parameters: 
	//		strDir			-	是新添加的要监视的文件夹
	bool startMonitorThread(CString strDir);
	//------------------------------------------------------------------------------
	//Summary: 
	//需要检查监视的文件夹是否已经在监视的文件夹或者子文件夹中，或者是包含了其中的一些文件夹
	//Parameters:
	//		strDir			-	要检查的路径
	//		dirs			-	如果发现指定路径包含了已经有的路径那么就通过dirs返回
	//Returns:
	//如果是需要添加该文件夹的监视，则返回0，
	//如果指定文件夹已经被监视，则返回1，
	//如果是包含了一些文件夹，那么要返回2，并返回这些被包含的文件夹路径。
	//如果发生其他错误，返回3
	int checkDir(CString strDir, std::vector<CString>& dirs);
	//Summary:
	//			添加一个监视信息
	//Parameters:
	//		pData			-	添加新的监视数据
	void addMonData(MonData* pData);
	//Summary:
	//			监视文件夹的线程
	static DWORD monitor(LPVOID param);
	//Summary:
	//			尝试删除关掉的线程的线程数据
	void deleteThreadInfo();
	
	//Summary:
	//	添加跟踪信息，为了调试使用
	static void pushTrackInfo(LPCTSTR info);
private:
	CRITICAL_SECTION	m_MonInfoCS;//负责保护监视信息数据
	std::set<MonData*> m_MonitorInfo;//!<记录监视信息
	std::set<MonData*> m_MonitorInfoToDel;//!<记录关闭掉线程之后，对应线程的数据，
};
///记录要监视情况的数据
struct MonData 
{
	//!监视线程的状态
	enum THRDSTAT{
		NORMAL = 0, //正常运行
		FAILED = 1, //运行失败
		CLOSED = 2, //线程关闭
		UNINIT = 3	//线程尚未运行
	};

	MonData()
	{
		_bstop = false;
		_dir = _T("");
		_thrdID = NULL;
		_stat = UNINIT;
	}
	~MonData(){}

	MonData(const MonData& obj)
	{
		_bstop = obj._bstop;
		_stat = obj._stat;
		_thrdID = obj._thrdID;
		_dir = obj._dir;
	}
	MonData& operator = (const MonData& obj)
	{
		if (this != &obj)
		{
			_bstop = obj._bstop;
			_stat = obj._stat;
			_thrdID = obj._thrdID;
			_dir = obj._dir;
		}
		return *this;
	}
	bool operator < (const MonData& obj)const
	{
		//一个监视数据的不同就是指其文件夹路径
		return _dir.Compare(obj._dir) < 0;
	}

	bool		_bstop;	//!<记录是否要正常运行的线程停止运行
	THRDSTAT 	_stat;	//!<记录线程的状态
	DWORD 		_thrdID;	//!<记录相关监视线程的ID
	CString		_dir;	//!<记录要监视的文件夹的路径
};
