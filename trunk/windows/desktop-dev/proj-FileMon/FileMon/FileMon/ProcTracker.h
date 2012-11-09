#pragma once
#include <list>
using namespace std;

//!brief
//  ���ฺ����ٳ����е�ִ�й��̣������ṩ���Ƕ��󼶱�ĸ���
//	����ģʽ

//!<���彫��ʷ��¼д���ļ���������ֵ
#define MAX_TRACKINFO		999


class ProcTracker
{
private:
	ProcTracker(void);
public:
	~ProcTracker(void);
public:
	//��������Ψһ�ķ���
	static ProcTracker* getTracker();
	//ɾ������
	static void deleteTracker();
	//ѯ���Ƿ����µ���Ϣ
	bool update();
	//ȡ����ǰ�ĸ�����Ϣ����������Ὣm_bUpdate����Ϊfalse
	void getTrackerInfo(std::list<CString>& trackInfo);
	//д�������Ϣ����������Ὣm_bUpdate����Ϊtrue
	void pushTrackInfo(CString& strTrack);
	//����ά������Ϣ����������
	void setUpperLimit(int nLimit);
	//�ж϶����Ƿ���Ч
	bool isValid();
private:
	//������Ϣ�������ޣ�д�뱾���ļ�
	void TrackInfo2File();
private:
	static ProcTracker* s_Tracker;
	CRITICAL_SECTION m_TrackInfoListCS;
	std::list<CString> m_TrackInfoList;	//!<��¼������Ϣ����������ֻά��һ����������Ϣ
	bool	m_bUpdate;					//!<��¼�Ƿ����µĸ�����Ϣ
	bool	m_bValid;					//!<��¼�Ƿ���Ч
	size_t	m_InfoUpperLimit;			//!<��¼������ֵ
};
