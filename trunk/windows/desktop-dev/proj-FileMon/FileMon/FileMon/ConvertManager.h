#pragma once

//���ฺ����ת������ĵ���
/*Ŀǰ��Ҫ���ȳ�����ȵĸ����У���ת���ļ��б�FileMonList:L��������ȷ�ConvertManager:M �� �ļ�ת����C
	������֮��ĵ��ȷ�ʽ�ǣ�
	L���𴴽�һ���¼�askForConvert�¼���
	M���𴴽�beginConvert�¼����ȴ�askForConvert�¼���finishConvert�¼���
	C���𴴽�finishConvert�¼����ȴ�beginConvert�¼���
	����M��C֮����Ҫʹ�ùܵ����������ݵ�ͨ��
*/


class ConvertManager
{
private:
	ConvertManager(void);
public:
	~ConvertManager(void);
public:
	//�õ����ȶ���
	static ConvertManager* getConvertManagerObj();
	//ɾ�����ȶ���
	static void deleteConvertManager();
	//��ʼ����
	void startManager();
	//�����̵߳�������
	static DWORD WINAPI ManagerThread(LPVOID param);
	//��ת��������������������������������ת�����ļ�����ת�����
	static void PostResult(CString& strFileName, bool bSuc);
private:
	void static pushTrackInfo(LPCTSTR info);
private:
	static ConvertManager* s_ConvertManager;
	static HANDLE s_hAskForConvert;	//!<��¼����ת�����¼����
	static HANDLE s_hBeginConvert;	//!<��¼��ʼת�����¼����
	static HANDLE s_hFinishConvert;	//!<��¼ת���������¼����
	static HANDLE s_hPipe;			//!<��¼M��C֮��ͨ�ŵĹܵ����
	bool m_bStartThread;	//!<��¼�߳��Ƿ��Ѿ�����
};
