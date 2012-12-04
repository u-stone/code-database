#pragma once

//该类负责做转换程序的调度
/*目前需要调度程序调度的各方有：待转换文件列表FileMonList:L，程序调度方ConvertManager:M 和 文件转换方C
	这三者之间的调度方式是：
	L负责创建一个事件askForConvert事件；
	M负责创建beginConvert事件，等待askForConvert事件和finishConvert事件；
	C负责创建finishConvert事件，等待beginConvert事件；
	其中M和C之间需要使用管道来进行数据的通信
*/


class ConvertManager
{
private:
	ConvertManager(void);
public:
	~ConvertManager(void);
public:
	//得到调度对象
	static ConvertManager* getConvertManagerObj();
	//删除调度对象
	static void deleteConvertManager();
	//开始调度
	void startManager();
	//调度线程的主程序
	static DWORD WINAPI ManagerThread(LPVOID param);
	//将转化结束结果传给服务器，传输的内容是转化的文件名和转换结果
	static void PostResult(CString& strFileName, bool bSuc);
private:
	void static pushTrackInfo(LPCTSTR info);
private:
	static ConvertManager* s_ConvertManager;
	static HANDLE s_hAskForConvert;	//!<记录请求转换的事件句柄
	static HANDLE s_hBeginConvert;	//!<记录开始转换的事件句柄
	static HANDLE s_hFinishConvert;	//!<记录转换结束的事件句柄
	static HANDLE s_hPipe;			//!<记录M和C之间通信的管道句柄
	bool m_bStartThread;	//!<记录线程是否已经启动
};
