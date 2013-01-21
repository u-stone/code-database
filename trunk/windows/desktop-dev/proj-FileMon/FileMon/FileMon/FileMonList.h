#pragma once
#include <algorithm>
#include <vector>
#include "SwitchBufServer.h"

using namespace std;


#define		MAXWAITLISTSIZE		999		//!<记录等待队列的最大数值，超过该值应该提出警报说明机器的性能不行，或者是应该修改程序以提高效率了
#define		MAXFINISHLISTSIZE	9999	//!<记录完成队列的最大数值，这个值可以取1000以上的值
#define		FINISHLISTHISTORY	_T("finishfilelist.txt")	//!<记录完成队列本地化的文件名
#define		SERIALIZEDFILES		_T("serializefilenames.txt")//!<记录上次由于等待队列中元素过多而未能存入队列，而写入本地文件的文件名数据
#define		MAXSERIALIZESIZE	999		//!< 记录存放着未能存入等待队列的数据 而缓存起来等待本地化或者写入等待队列的数据
#define		TARGETCONVERTEDFILE	L"emf"	//!< 记录将要转化为什么类型的文件

class FileMonList;

//用于线程操作数据的交换信息
struct BufInfoForThread
{
	FileMonList*		_FileMonList;	
	CRITICAL_SECTION*	_pcs;			//这个是向FileMonList的等待列表输出数据的关键区域保护段
	SwitchBufServer*	_pSwitchBuf;	//这里负责接受外部数据，并负责向FileMonList中输出数据
	HANDLE				_hBuf2ListEvent;//这个事件负责将SwitchBuf中的内容添加到FileMonList中的同步，一旦有BUF中有数据，就将该事件激活，这样FileMonList可以接受数据了
};

//负责维护一个将要转化的文件的队列。
//把文件队列分为等待队列和已完成队列是为了避免出现重复转化的情况
//使用单件模式


class FileMonList
{
private:
	FileMonList(void);
public:
	~FileMonList(void);

	//!< brief 这里记录可以添加到文件列表中的文件类型，如果是UnknownFile就忽略，其他类型就处理
	enum FileType{
		UnknownFile = 0,
		DwgFile = 1,
		PdfFile = 2
	};
public:
	//Summary:
	//		得到FileMonList对象的唯一方法
	static FileMonList* getFileListObj();
	//Summary:
	//		使用完毕的时候删除对象
	static void deleteListObj();
	//Summary:
	//		添加新的待转化路径到s_FileWaitList中，当然先要检查该文件路径是否已经在两个队列里面出现
	bool addMonFilePath(CString& strPath);
	//Summary:
	//		将已经完成操作的文件路径从s_FileWaitList中取出来，放在s_FileFinishList中
	bool finishOpFilePath(CString& strPath);
	//Summary:
	//		清空队列数据
	void clear();
	//Summary:
	//		得到下一个待操作的文件的名称
	bool getNextFileObj(CString& strFilePath);
	//Summary:
	//		设置监视的文件名的后缀过滤
	void setFileFilter(CString& strFileExt);
	//Summary:
	//		设置待转化的文件列表的上限
	void setWaitFileListMaxSize(size_t mz);
	//Summary:
	//		设置完成队列中的数量上限
	void setFinishListMaxSize(size_t mz);

private:
	//Summary:
	//		这个是将数据从SwitchBuf放入FileMonList的线程
	static DWORD transDataFromBuf2MonList(void* pParam);
	//Summary:
	//		添加跟踪信息，为了调试使用
	static void pushTrackInfo(LPCTSTR info);
	//Summary:
	//		将给定数据(文件路径)添加到等待列表中，这个函数会被阻塞
	void addFilePathToList(CString strFilePath);
	//Summary:
	//		操作完成之后的检查
	BOOL checkFinishOp(CString strParam);
	//Summary:
	//		检查完成队列中的数据量：比如完成队列里面数据太多了，需要将数据写入本地文件等
	void checkFinishListSize();
	//初始化关键区域和通知调度的事件
	void init();
	//检查文件类型是否合格，合格返回true，不合格返回false，就是检查后缀名
	FileType checkFileType(CString& strFilePath);
	//Summary:
	//		这个函数就在添加文件的时候使用，尝试将数据存入等待队列
	//rule:	当等待列表中变满时，尝试将数据存入等待队列，如果等待队列已满，那么就存入序列化队列中，
	//		如果序列化队列已满，那么就将数据本地化，保存至SERIALIZEDFILES文件中
	void cacheFilePath(CString& strFilePath);
	//Summary:
	//		这个函数在等待列表中数据减少的时候使用，尝试从缓存的数据中取出数据来放入等待队列
	//rule:	当等待列表中变空时，尝试从本地序列化文件SERIALIZEDFILES中取出数据来放入等待队列
	//		如果本地序列化文件中没有数据了，再从序列化队列中取出数据来放入等待队列
	void fetchFilePath();
// 	//Summary:
// 	//		运行起php-cgi
// 	void startPhpCgi();
// 	//Summary:
// 	//		向php-cgi传递参数, 
// 	//param:
// 	//		strFilePath	转换完成的文件路径
// 	//		strRes	转换的结果，成功返回1，失败返回0
// 	void sendData2Php(CString strFilePath, CString strRes);
// 	//Summary:
// 	//		向php-cgi收取数据
// 	void recvDataFromPhp(CString& strData);

private:
	static FileMonList* s_Obj;

	BOOL				m_bDataInFile;		//记录本地文件中是否有数据
	size_t				m_WaitListMaxSize;	//设置队列的上限，超过上限要存入序列化队列
	size_t				m_SerializeListMaxSize;	//设置队列的上限，超过上限要存入本地
	size_t				m_FinishListMaxSize;//设置完成队列中数据量的上限
	//CString				m_strFileExt;		//监视的文件的文件名后缀过滤
	HANDLE				m_hStdoutR;			//输出数据到php
	HANDLE				m_hStdoutW;			//输出数据到php
	HANDLE				m_hAskForConvert;	//记录请求文件转换事件
	CRITICAL_SECTION 	m_WaitListCS;		//操作添加文件路径的关键段
	CRITICAL_SECTION 	m_finishListCS;		//操作完成转化的文件路径的关键段
	std::vector<CString> m_FileWaitList;	//待转化的文件列表
	std::vector<CString> m_FileSerializeList;//待序列化的文件列表
	std::vector<CString> m_FileFinishList;	//已经完成转化的文件列表
	SwitchBufServer		*m_pSwitchBuf;		//保存创建出来的缓存对象
	BufInfoForThread	m_TransDataThreadInfo;	//这个是移动buf中的数据到FileMonList中的线程需要的参数信息
};
