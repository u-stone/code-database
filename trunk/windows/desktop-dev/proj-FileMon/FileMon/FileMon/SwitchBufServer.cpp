#include "StdAfx.h"
#include "SwitchBufServer.h"
#include "FileMonList.h"

SwitchBufServer::SwitchBufServer(void)
{
	init();
}

SwitchBufServer::SwitchBufServer(CString str)
{
	init(str);
}

SwitchBufServer::~SwitchBufServer(void)
{
}

void SwitchBufServer::init(CString str)
{
	m_IndexBuf2Cache = 1;
	m_enterWriteDataSection = 0;
	//创建事件
	SECURITY_ATTRIBUTES sa;
	memset(&sa, 0, sizeof(SECURITY_ATTRIBUTES));
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	m_hBuf2CacheEvent = CreateEvent(&sa, TRUE, FALSE, str);
	if (m_hBuf2CacheEvent == NULL)
		pushTrackInfo(L"创建事件buf2cacheevent失败");
	m_hTrySwitchEvent = CreateEvent(&sa, TRUE, FALSE, TRYSWITCHEVENT);
	if (m_hTrySwitchEvent == NULL)
		pushTrackInfo(L"创建事件trytoswitchevent失败");
}
void SwitchBufServer::pushData(CString strData)
{
	//理论上这里发生的情况应该是buf[0]不能进入的话，就可以进入buf[1]，反之亦然，其他地方需要保证的就是不能同时锁定两个关键区域，
	//	但是一旦进入某一个缓存中操作数据就必须保证其他线程不能写(删除/插入数据)该缓存。
	//同时由于线程的原因，可能会出现pushData函数使用中被挂起，而造成buf[0]进不去，buf[1]也进不去的情况，这里做了一个m_enterPushDataSection标记
	//其他地方需要做好判断保证不会在m_enterPushDataSection为1的时候，修改buf的关键区段
	ResetEvent(m_hTrySwitchEvent);
	//pushTrackInfo(L"插入数据" + strData);//绝对不能调用pushTrackInfo()，这个函数会造成阻塞
	InterlockedExchange(&m_enterWriteDataSection, 1);
	m_SwitchBuf[1-m_IndexBuf2Cache]._buf.push_back(strData);
	m_SwitchBuf[1-m_IndexBuf2Cache]._bEmpty = false;//状态为空
	InterlockedExchange(&m_enterWriteDataSection, 0);
	SetEvent(m_hTrySwitchEvent);
	SetEvent(m_hBuf2CacheEvent);//提醒可以将buf中的数据刷入cache中，这个是提示外部调用者
	//adjustStatus();
}

bool SwitchBufServer::getData(CString& strData)
{
	ResetEvent(m_hTrySwitchEvent);
	if (!m_SwitchBuf[m_IndexBuf2Cache]._bEmpty && m_enterWriteDataSection == 0)
	{
		InterlockedExchange(&m_enterWriteDataSection, 1);
		//将可以导出的数据m_SwitchBuf[m_IndexBuf2Cache]._buf返回并删除数组中该记录
		//这里使用的是一个双端队列
		pushTrackInfo(L"从SwitchBuf中取出数据");
		strData = m_SwitchBuf[m_IndexBuf2Cache]._buf.front();
		m_SwitchBuf[m_IndexBuf2Cache]._buf.pop_front();
		if (m_SwitchBuf[m_IndexBuf2Cache]._buf.empty())//如果没有数据的话就需要将状态置为空
			m_SwitchBuf[m_IndexBuf2Cache]._bEmpty = true;
		InterlockedExchange(&m_enterWriteDataSection, 0);
	}
	SetEvent(m_hTrySwitchEvent);
	return adjustStatus();
}

bool SwitchBufServer::adjustStatus()
{
	//根据状态进行处理
	if (m_SwitchBuf[m_IndexBuf2Cache]._bEmpty)//也就是说out-buf为空
	{
		if (!m_SwitchBuf[1-m_IndexBuf2Cache]._bEmpty)//in-buf不为空
		{
			//旋转buf-G
			//旋转的时候需要保证两个buf都没有被线程操作。这里保证这一点的方法是检查m_enterWriteDataSection状态
			if (m_enterWriteDataSection == 1)//表示正在操作buf，这时候不能切换buf，否则会出现访问冲突
			{
				pushTrackInfo(L"等待 旋转SwitchBuf");
				//这时候应该等待
				if (WaitForSingleObject(m_hTrySwitchEvent, INFINITE) == WAIT_OBJECT_0)
				{
					pushTrackInfo(L"旋转SwitchBuf");
					InterlockedExchange(&m_IndexBuf2Cache, 1-m_IndexBuf2Cache);
				}
			}
			else
			{
				pushTrackInfo(L"直接旋转SwitchBuf");
				InterlockedExchange(&m_IndexBuf2Cache, 1-m_IndexBuf2Cache);
			}
			return true;
		}
		//else //就是说in-buf也为空，那么不做任何操作
	}
	else// if (!m_SwitchBuf[m_IndexBuf2Cache]._bEmpty)//也就是说out-buf不为空，那么可以将数据导出到外部cache中
	{
		//此时需要执行将数据导出到cache的操作
		pushTrackInfo(L"激活事件 准备将数据从buf中转移到cache中");
		SetEvent(m_hBuf2CacheEvent);
	}
	return false;
}

void SwitchBufServer::pushTrackInfo(LPCTSTR info)
{
	SWITCHBUFSERVER;
	static CString str;
	str.Empty();
	str = _T("SwitchBufServer: ");
	str += info;
	ProcTracker::getTracker()->pushTrackInfo(str);
}