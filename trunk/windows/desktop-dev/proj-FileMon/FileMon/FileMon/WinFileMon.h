#pragma once
#include <set>
#include <vector>
using namespace std;

struct MonData;

class ProcTracker;

//!WinFileMon��
/*!
	���������û���ӻ���ɾ������·���ġ���������ֹͣ��Ӧ�ļ����߳�.
*/
class WinFileMon
{
public:
	WinFileMon(void);
	~WinFileMon(void);
public:
	//Summary:
	//	���Ҫ���ӵ��ļ���strDir���������߳̿�ʼ����
	bool addMonitorDir(CString strDir);
	//Summary:
	//	ɾ��Ҫ���ӵ��ļ���strDir
	void removeMonitorDir(CString strDir);
private:
	//Summary:	�����߳�ִ���µ��ļ��м���
	//Parameters: 
	//		strDir			-	������ӵ�Ҫ���ӵ��ļ���
	bool startMonitorThread(CString strDir);
	//------------------------------------------------------------------------------
	//Summary: 
	//��Ҫ�����ӵ��ļ����Ƿ��Ѿ��ڼ��ӵ��ļ��л������ļ����У������ǰ��������е�һЩ�ļ���
	//Parameters:
	//		strDir			-	Ҫ����·��
	//		dirs			-	�������ָ��·���������Ѿ��е�·����ô��ͨ��dirs����
	//Returns:
	//�������Ҫ��Ӹ��ļ��еļ��ӣ��򷵻�0��
	//���ָ���ļ����Ѿ������ӣ��򷵻�1��
	//����ǰ�����һЩ�ļ��У���ôҪ����2����������Щ���������ļ���·����
	//��������������󣬷���3
	int checkDir(CString strDir, std::vector<CString>& dirs);
	//Summary:
	//			���һ��������Ϣ
	//Parameters:
	//		pData			-	����µļ�������
	void addMonData(MonData* pData);
	//Summary:
	//			�����ļ��е��߳�
	static DWORD monitor(LPVOID param);
	//Summary:
	//			����ɾ���ص����̵߳��߳�����
	void deleteThreadInfo();
	
	//Summary:
	//	��Ӹ�����Ϣ��Ϊ�˵���ʹ��
	static void pushTrackInfo(LPCTSTR info);
private:
	CRITICAL_SECTION	m_MonInfoCS;//���𱣻�������Ϣ����
	std::set<MonData*> m_MonitorInfo;//!<��¼������Ϣ
	std::set<MonData*> m_MonitorInfoToDel;//!<��¼�رյ��߳�֮�󣬶�Ӧ�̵߳����ݣ�
};
///��¼Ҫ�������������
struct MonData 
{
	//!�����̵߳�״̬
	enum THRDSTAT{
		NORMAL = 0, //��������
		FAILED = 1, //����ʧ��
		CLOSED = 2, //�̹߳ر�
		UNINIT = 3	//�߳���δ����
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
		//һ���������ݵĲ�ͬ����ָ���ļ���·��
		return _dir.Compare(obj._dir) < 0;
	}

	bool		_bstop;	//!<��¼�Ƿ�Ҫ�������е��߳�ֹͣ����
	THRDSTAT 	_stat;	//!<��¼�̵߳�״̬
	DWORD 		_thrdID;	//!<��¼��ؼ����̵߳�ID
	CString		_dir;	//!<��¼Ҫ���ӵ��ļ��е�·��
};
