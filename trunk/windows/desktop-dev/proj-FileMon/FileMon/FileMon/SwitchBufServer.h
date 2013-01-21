#pragma once
#include <deque>

//!<	该类提供一个拥有两个缓存队列的的缓存组，当其中外部程序写入其中一个缓存的时候，
//		同时允许访问另一个缓存中的数据，并且对外进行封装，实现的效果是：可以无阻塞地存入数据，而后有提醒地取出数据并进行清理
//		这里提醒的方式就是命名事件BUF2CACHEEVENT

//!< brief: 带标记功能的缓存，可以记录缓存是否为空

#define   TRYSWITCHEVENT		_T("trytoswitchevent")

class ProcTracker;

class MarkBuf 
{
public:
	MarkBuf(){
		_bEmpty = true;
	}
	MarkBuf(const MarkBuf& obj){
		_bEmpty = obj._bEmpty;
		_buf.assign(obj._buf.begin(), obj._buf.end());
	}
	MarkBuf& operator = (const MarkBuf& obj){
		if (this != &obj)
		{
			_bEmpty = obj._bEmpty;
			_buf.assign(obj._buf.begin(), obj._buf.end());
		}
		return *this;
	}
public:
	bool _bEmpty;
	std::deque<CString> _buf;
};


class SwitchBufServer
{
public:
	SwitchBufServer(void);//这个构造函数会构造一个起好名字的BUF到cache的提醒事件
	SwitchBufServer(CString str);//这个构造函数会构造一个名为str的BUF到cache的提醒事件
	~SwitchBufServer(void);

public:
	//Summary:
	//		将数据存入当前可用的缓存中
	//rule:	这个函数不会有阻塞，并且不能产生数据访问冲突
	void pushData(CString strData);
	//Summary:
	//		将数据从缓存中取出来，并删除该数据在缓存中的记录，这个将数据放入cache中
	//rule:	该函数不会有阻塞
	//return:
	//		如果该函数旋转了buf，那么会返回true，否则返回false，具体的数据在strData中
	//note:	这两个函数(pushData和getData是由两个线程调用的)
	bool getData(CString& strData);

private:
	//Summary:
	//		初始化
	void init(CString str = BUF2CACHEEVENT);
	//Summary:
	//		检查当前的状态，看需要进行什么样的操作，这个由pushData和getData两个函数调用，用来检查应该转到哪个状态，进行调节
	//rule:	这个根据两个缓存为读还是写，分3种情况:
	/*
			op					供外部写入数据的buf(in-buf)状态				将数据导出到cache的buf(out-buf)状态
	  [旋转m_SwitchBuf]			      不为空							      为空
	       等待						   为空								      为空
	    执行导出到cache				  空/不空							     不为空
	*/
	//Return:
	//		如果旋转了buf，那么就返回true，否则返回false，
	//Note:	这个函数会产生阻塞
	bool adjustStatus();

	//Summary:
	//	添加跟踪信息，为了调试使用，这个函数会造成阻塞
	static void pushTrackInfo(LPCTSTR info);

private:
	//Summary:
	//		这是关键，存放外部存入的数据并且要记录是否为空
	MarkBuf m_SwitchBuf[2];			//!< 这个叫做buf-G
	volatile LONG m_IndexBuf2Cache;	//!< 记录out-buf的下标，默认为1
	volatile LONG m_enterWriteDataSection;	//!< 记录进入写缓存数据的区间，为1表示正在操作，为0表示没有操作，同时也就是说正在使用m_IndexBuf2Cache当前的值
	HANDLE m_hTrySwitchEvent;		//!< 记录通知线程可以旋转两个buf的用途
	HANDLE m_hBuf2CacheEvent;		//!< 记录将out-buf中的数据写入cache的事件
};
