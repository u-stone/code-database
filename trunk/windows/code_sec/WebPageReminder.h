#pragma once

//�����������ָ����webҳ���������
//���������ڲ�ά��һ��url�б������б��е�url�Ѿ���ʼ���ж�ʱ��������

struct RefreshInfo{
	CString strUrl;

	//RefreshInfo(){
	//	
	//}
	//~RefreshInfo(){
	//	
	//}
	//RefreshInfo(const RefreshInfo& obj){
	//	strUrl = obj.strUrl;
	//}
	//	
	//RefreshInfo operator = (const RefreshInfo& obj){
	//	if (this != &obj)
	//		strUrl = obj.strUrl;
	//	return *this;
	//}
};

class WebPageReminder
{
private:
	WebPageReminder(void);
	~WebPageReminder(void);
public:
	static WebPageReminder* GetInstance(){
		if(s_Obj == NULL){
			s_Obj = new WebPageReminder();
		}
		return s_Obj;
	}
	static void DeleteInstance(){
		delete s_Obj;
	}
	void Refresh(CString strUrl);
private:
	std::vector<RefreshInfo*> m_RefreshInfo;
	static WebPageReminder* s_Obj;
};

