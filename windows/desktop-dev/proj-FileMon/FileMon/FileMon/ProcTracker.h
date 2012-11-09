#pragma once
#include <list>
using namespace std;

//!brief
//  该类负责跟踪程序中的执行过程，这里提供的是对象级别的跟踪
//	单件模式

//!<定义将历史记录写入文件的上限阈值
#define MAX_TRACKINFO		999


class ProcTracker
{
private:
	ProcTracker(void);
public:
	~ProcTracker(void);
public:
	//创建对象唯一的方法
	static ProcTracker* getTracker();
	//删除对象
	static void deleteTracker();
	//询问是否有新的信息
	bool update();
	//取出当前的跟踪信息，这个函数会将m_bUpdate设置为false
	void getTrackerInfo(std::list<CString>& trackInfo);
	//写入跟踪信息，这个函数会将m_bUpdate设置为true
	void pushTrackInfo(CString& strTrack);
	//设置维护的信息的数量上限
	void setUpperLimit(int nLimit);
	//判断对象是否有效
	bool isValid();
private:
	//跟踪信息超过上限，写入本地文件
	void TrackInfo2File();
private:
	static ProcTracker* s_Tracker;
	CRITICAL_SECTION m_TrackInfoListCS;
	std::list<CString> m_TrackInfoList;	//!<记录跟踪信息，但是这里只维护一定数量的信息
	bool	m_bUpdate;					//!<记录是否有新的更新信息
	bool	m_bValid;					//!<记录是否有效
	size_t	m_InfoUpperLimit;			//!<记录上限数值
};
