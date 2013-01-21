#pragma once
#include <algorithm>
#include <vector>
#include "SwitchBufServer.h"

using namespace std;


#define		MAXWAITLISTSIZE		999		//!<��¼�ȴ����е������ֵ��������ֵӦ���������˵�����������ܲ��У�������Ӧ���޸ĳ��������Ч����
#define		MAXFINISHLISTSIZE	9999	//!<��¼��ɶ��е������ֵ�����ֵ����ȡ1000���ϵ�ֵ
#define		FINISHLISTHISTORY	_T("finishfilelist.txt")	//!<��¼��ɶ��б��ػ����ļ���
#define		SERIALIZEDFILES		_T("serializefilenames.txt")//!<��¼�ϴ����ڵȴ�������Ԫ�ع����δ�ܴ�����У���д�뱾���ļ����ļ�������
#define		MAXSERIALIZESIZE	999		//!< ��¼�����δ�ܴ���ȴ����е����� �����������ȴ����ػ�����д��ȴ����е�����
#define		TARGETCONVERTEDFILE	L"emf"	//!< ��¼��Ҫת��Ϊʲô���͵��ļ�

class FileMonList;

//�����̲߳������ݵĽ�����Ϣ
struct BufInfoForThread
{
	FileMonList*		_FileMonList;	
	CRITICAL_SECTION*	_pcs;			//�������FileMonList�ĵȴ��б�������ݵĹؼ����򱣻���
	SwitchBufServer*	_pSwitchBuf;	//���︺������ⲿ���ݣ���������FileMonList���������
	HANDLE				_hBuf2ListEvent;//����¼�����SwitchBuf�е�������ӵ�FileMonList�е�ͬ����һ����BUF�������ݣ��ͽ����¼��������FileMonList���Խ���������
};

//����ά��һ����Ҫת�����ļ��Ķ��С�
//���ļ����з�Ϊ�ȴ����к�����ɶ�����Ϊ�˱�������ظ�ת�������
//ʹ�õ���ģʽ


class FileMonList
{
private:
	FileMonList(void);
public:
	~FileMonList(void);

	//!< brief �����¼������ӵ��ļ��б��е��ļ����ͣ������UnknownFile�ͺ��ԣ��������;ʹ���
	enum FileType{
		UnknownFile = 0,
		DwgFile = 1,
		PdfFile = 2
	};
public:
	//Summary:
	//		�õ�FileMonList�����Ψһ����
	static FileMonList* getFileListObj();
	//Summary:
	//		ʹ����ϵ�ʱ��ɾ������
	static void deleteListObj();
	//Summary:
	//		����µĴ�ת��·����s_FileWaitList�У���Ȼ��Ҫ�����ļ�·���Ƿ��Ѿ������������������
	bool addMonFilePath(CString& strPath);
	//Summary:
	//		���Ѿ���ɲ������ļ�·����s_FileWaitList��ȡ����������s_FileFinishList��
	bool finishOpFilePath(CString& strPath);
	//Summary:
	//		��ն�������
	void clear();
	//Summary:
	//		�õ���һ�����������ļ�������
	bool getNextFileObj(CString& strFilePath);
	//Summary:
	//		���ü��ӵ��ļ����ĺ�׺����
	void setFileFilter(CString& strFileExt);
	//Summary:
	//		���ô�ת�����ļ��б������
	void setWaitFileListMaxSize(size_t mz);
	//Summary:
	//		������ɶ����е���������
	void setFinishListMaxSize(size_t mz);

private:
	//Summary:
	//		����ǽ����ݴ�SwitchBuf����FileMonList���߳�
	static DWORD transDataFromBuf2MonList(void* pParam);
	//Summary:
	//		��Ӹ�����Ϣ��Ϊ�˵���ʹ��
	static void pushTrackInfo(LPCTSTR info);
	//Summary:
	//		����������(�ļ�·��)��ӵ��ȴ��б��У���������ᱻ����
	void addFilePathToList(CString strFilePath);
	//Summary:
	//		�������֮��ļ��
	BOOL checkFinishOp(CString strParam);
	//Summary:
	//		�����ɶ����е���������������ɶ�����������̫���ˣ���Ҫ������д�뱾���ļ���
	void checkFinishListSize();
	//��ʼ���ؼ������֪ͨ���ȵ��¼�
	void init();
	//����ļ������Ƿ�ϸ񣬺ϸ񷵻�true�����ϸ񷵻�false�����Ǽ���׺��
	FileType checkFileType(CString& strFilePath);
	//Summary:
	//		���������������ļ���ʱ��ʹ�ã����Խ����ݴ���ȴ�����
	//rule:	���ȴ��б��б���ʱ�����Խ����ݴ���ȴ����У�����ȴ�������������ô�ʹ������л������У�
	//		������л�������������ô�ͽ����ݱ��ػ���������SERIALIZEDFILES�ļ���
	void cacheFilePath(CString& strFilePath);
	//Summary:
	//		��������ڵȴ��б������ݼ��ٵ�ʱ��ʹ�ã����Դӻ����������ȡ������������ȴ�����
	//rule:	���ȴ��б��б��ʱ�����Դӱ������л��ļ�SERIALIZEDFILES��ȡ������������ȴ�����
	//		����������л��ļ���û�������ˣ��ٴ����л�������ȡ������������ȴ�����
	void fetchFilePath();
// 	//Summary:
// 	//		������php-cgi
// 	void startPhpCgi();
// 	//Summary:
// 	//		��php-cgi���ݲ���, 
// 	//param:
// 	//		strFilePath	ת����ɵ��ļ�·��
// 	//		strRes	ת���Ľ�����ɹ�����1��ʧ�ܷ���0
// 	void sendData2Php(CString strFilePath, CString strRes);
// 	//Summary:
// 	//		��php-cgi��ȡ����
// 	void recvDataFromPhp(CString& strData);

private:
	static FileMonList* s_Obj;

	BOOL				m_bDataInFile;		//��¼�����ļ����Ƿ�������
	size_t				m_WaitListMaxSize;	//���ö��е����ޣ���������Ҫ�������л�����
	size_t				m_SerializeListMaxSize;	//���ö��е����ޣ���������Ҫ���뱾��
	size_t				m_FinishListMaxSize;//������ɶ�����������������
	//CString				m_strFileExt;		//���ӵ��ļ����ļ�����׺����
	HANDLE				m_hStdoutR;			//������ݵ�php
	HANDLE				m_hStdoutW;			//������ݵ�php
	HANDLE				m_hAskForConvert;	//��¼�����ļ�ת���¼�
	CRITICAL_SECTION 	m_WaitListCS;		//��������ļ�·���Ĺؼ���
	CRITICAL_SECTION 	m_finishListCS;		//�������ת�����ļ�·���Ĺؼ���
	std::vector<CString> m_FileWaitList;	//��ת�����ļ��б�
	std::vector<CString> m_FileSerializeList;//�����л����ļ��б�
	std::vector<CString> m_FileFinishList;	//�Ѿ����ת�����ļ��б�
	SwitchBufServer		*m_pSwitchBuf;		//���洴�������Ļ������
	BufInfoForThread	m_TransDataThreadInfo;	//������ƶ�buf�е����ݵ�FileMonList�е��߳���Ҫ�Ĳ�����Ϣ
};
