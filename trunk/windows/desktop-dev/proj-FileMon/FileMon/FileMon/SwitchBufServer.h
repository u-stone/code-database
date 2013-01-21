#pragma once
#include <deque>

//!<	�����ṩһ��ӵ������������еĵĻ����飬�������ⲿ����д������һ�������ʱ��
//		ͬʱ���������һ�������е����ݣ����Ҷ�����з�װ��ʵ�ֵ�Ч���ǣ������������ش������ݣ����������ѵ�ȡ�����ݲ���������
//		�������ѵķ�ʽ���������¼�BUF2CACHEEVENT

//!< brief: ����ǹ��ܵĻ��棬���Լ�¼�����Ƿ�Ϊ��

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
	SwitchBufServer(void);//������캯���ṹ��һ��������ֵ�BUF��cache�������¼�
	SwitchBufServer(CString str);//������캯���ṹ��һ����Ϊstr��BUF��cache�������¼�
	~SwitchBufServer(void);

public:
	//Summary:
	//		�����ݴ��뵱ǰ���õĻ�����
	//rule:	����������������������Ҳ��ܲ������ݷ��ʳ�ͻ
	void pushData(CString strData);
	//Summary:
	//		�����ݴӻ�����ȡ��������ɾ���������ڻ����еļ�¼����������ݷ���cache��
	//rule:	�ú�������������
	//return:
	//		����ú�����ת��buf����ô�᷵��true�����򷵻�false�������������strData��
	//note:	����������(pushData��getData���������̵߳��õ�)
	bool getData(CString& strData);

private:
	//Summary:
	//		��ʼ��
	void init(CString str = BUF2CACHEEVENT);
	//Summary:
	//		��鵱ǰ��״̬������Ҫ����ʲô���Ĳ����������pushData��getData�����������ã��������Ӧ��ת���ĸ�״̬�����е���
	//rule:	���������������Ϊ������д����3�����:
	/*
			op					���ⲿд�����ݵ�buf(in-buf)״̬				�����ݵ�����cache��buf(out-buf)״̬
	  [��תm_SwitchBuf]			      ��Ϊ��							      Ϊ��
	       �ȴ�						   Ϊ��								      Ϊ��
	    ִ�е�����cache				  ��/����							     ��Ϊ��
	*/
	//Return:
	//		�����ת��buf����ô�ͷ���true�����򷵻�false��
	//Note:	����������������
	bool adjustStatus();

	//Summary:
	//	��Ӹ�����Ϣ��Ϊ�˵���ʹ�ã�����������������
	static void pushTrackInfo(LPCTSTR info);

private:
	//Summary:
	//		���ǹؼ�������ⲿ��������ݲ���Ҫ��¼�Ƿ�Ϊ��
	MarkBuf m_SwitchBuf[2];			//!< �������buf-G
	volatile LONG m_IndexBuf2Cache;	//!< ��¼out-buf���±꣬Ĭ��Ϊ1
	volatile LONG m_enterWriteDataSection;	//!< ��¼����д�������ݵ����䣬Ϊ1��ʾ���ڲ�����Ϊ0��ʾû�в�����ͬʱҲ����˵����ʹ��m_IndexBuf2Cache��ǰ��ֵ
	HANDLE m_hTrySwitchEvent;		//!< ��¼֪ͨ�߳̿�����ת����buf����;
	HANDLE m_hBuf2CacheEvent;		//!< ��¼��out-buf�е�����д��cache���¼�
};
