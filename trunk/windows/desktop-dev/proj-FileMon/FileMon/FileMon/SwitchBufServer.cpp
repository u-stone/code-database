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
	//�����¼�
	SECURITY_ATTRIBUTES sa;
	memset(&sa, 0, sizeof(SECURITY_ATTRIBUTES));
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	m_hBuf2CacheEvent = CreateEvent(&sa, TRUE, FALSE, str);
	if (m_hBuf2CacheEvent == NULL)
		pushTrackInfo(L"�����¼�buf2cacheeventʧ��");
	m_hTrySwitchEvent = CreateEvent(&sa, TRUE, FALSE, TRYSWITCHEVENT);
	if (m_hTrySwitchEvent == NULL)
		pushTrackInfo(L"�����¼�trytoswitcheventʧ��");
}
void SwitchBufServer::pushData(CString strData)
{
	//���������﷢�������Ӧ����buf[0]���ܽ���Ļ����Ϳ��Խ���buf[1]����֮��Ȼ�������ط���Ҫ��֤�ľ��ǲ���ͬʱ���������ؼ�����
	//	����һ������ĳһ�������в������ݾͱ��뱣֤�����̲߳���д(ɾ��/��������)�û��档
	//ͬʱ�����̵߳�ԭ�򣬿��ܻ����pushData����ʹ���б����𣬶����buf[0]����ȥ��buf[1]Ҳ����ȥ���������������һ��m_enterPushDataSection���
	//�����ط���Ҫ�����жϱ�֤������m_enterPushDataSectionΪ1��ʱ���޸�buf�Ĺؼ�����
	ResetEvent(m_hTrySwitchEvent);
	//pushTrackInfo(L"��������" + strData);//���Բ��ܵ���pushTrackInfo()������������������
	InterlockedExchange(&m_enterWriteDataSection, 1);
	m_SwitchBuf[1-m_IndexBuf2Cache]._buf.push_back(strData);
	m_SwitchBuf[1-m_IndexBuf2Cache]._bEmpty = false;//״̬Ϊ��
	InterlockedExchange(&m_enterWriteDataSection, 0);
	SetEvent(m_hTrySwitchEvent);
	SetEvent(m_hBuf2CacheEvent);//���ѿ��Խ�buf�е�����ˢ��cache�У��������ʾ�ⲿ������
	//adjustStatus();
}

bool SwitchBufServer::getData(CString& strData)
{
	ResetEvent(m_hTrySwitchEvent);
	if (!m_SwitchBuf[m_IndexBuf2Cache]._bEmpty && m_enterWriteDataSection == 0)
	{
		InterlockedExchange(&m_enterWriteDataSection, 1);
		//�����Ե���������m_SwitchBuf[m_IndexBuf2Cache]._buf���ز�ɾ�������иü�¼
		//����ʹ�õ���һ��˫�˶���
		pushTrackInfo(L"��SwitchBuf��ȡ������");
		strData = m_SwitchBuf[m_IndexBuf2Cache]._buf.front();
		m_SwitchBuf[m_IndexBuf2Cache]._buf.pop_front();
		if (m_SwitchBuf[m_IndexBuf2Cache]._buf.empty())//���û�����ݵĻ�����Ҫ��״̬��Ϊ��
			m_SwitchBuf[m_IndexBuf2Cache]._bEmpty = true;
		InterlockedExchange(&m_enterWriteDataSection, 0);
	}
	SetEvent(m_hTrySwitchEvent);
	return adjustStatus();
}

bool SwitchBufServer::adjustStatus()
{
	//����״̬���д���
	if (m_SwitchBuf[m_IndexBuf2Cache]._bEmpty)//Ҳ����˵out-bufΪ��
	{
		if (!m_SwitchBuf[1-m_IndexBuf2Cache]._bEmpty)//in-buf��Ϊ��
		{
			//��תbuf-G
			//��ת��ʱ����Ҫ��֤����buf��û�б��̲߳��������ﱣ֤��һ��ķ����Ǽ��m_enterWriteDataSection״̬
			if (m_enterWriteDataSection == 1)//��ʾ���ڲ���buf����ʱ�����л�buf���������ַ��ʳ�ͻ
			{
				pushTrackInfo(L"�ȴ� ��תSwitchBuf");
				//��ʱ��Ӧ�õȴ�
				if (WaitForSingleObject(m_hTrySwitchEvent, INFINITE) == WAIT_OBJECT_0)
				{
					pushTrackInfo(L"��תSwitchBuf");
					InterlockedExchange(&m_IndexBuf2Cache, 1-m_IndexBuf2Cache);
				}
			}
			else
			{
				pushTrackInfo(L"ֱ����תSwitchBuf");
				InterlockedExchange(&m_IndexBuf2Cache, 1-m_IndexBuf2Cache);
			}
			return true;
		}
		//else //����˵in-bufҲΪ�գ���ô�����κβ���
	}
	else// if (!m_SwitchBuf[m_IndexBuf2Cache]._bEmpty)//Ҳ����˵out-buf��Ϊ�գ���ô���Խ����ݵ������ⲿcache��
	{
		//��ʱ��Ҫִ�н����ݵ�����cache�Ĳ���
		pushTrackInfo(L"�����¼� ׼�������ݴ�buf��ת�Ƶ�cache��");
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