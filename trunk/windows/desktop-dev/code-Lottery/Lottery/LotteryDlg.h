
// LotteryDlg.h : ͷ�ļ�
//

#pragma once
#define PICNUM			11
#define DIGITALNUM		10
#define PICNUMOFRING	20	//����ÿ��ѭ����ȡ��ͼƬ����

#define TIMER1			100	//���嶨ʱ��1���ʱ��
#define TIMER2			100	//���嶨ʱ��2���ʱ��

#define BOTTOMRATIO		(0.84)
#define TOPRATIO		(0.16)
#define HEIGHTRATIO		(BOTTOMRATIO - TOPRATIO)
#include <set>
#include <vector>
#include <algorithm>
#include <list>
#include <fstream>
#include <string>
//#include "SwitchList2.h"
using namespace std;

//��ż���ͼƬ����Ϣ
struct PicInfo 
{
	IPicture* _iPic;
	IStream* _iStream;
	std::wstring _strFilePath;
	HGLOBAL _hGlobal;
	int _width;
	int _height;
	OLE_XPOS_HIMETRIC _x;
	OLE_YPOS_HIMETRIC _y;
	OLE_XSIZE_HIMETRIC _cx;
	OLE_YSIZE_HIMETRIC _cy;

	PicInfo()
	{
		_iPic = NULL;
	}

	PicInfo& operator = (const PicInfo& obj)
	{
		if (this != &obj)
		{
			_iPic = obj._iPic;
			_iStream = obj._iStream;
			_strFilePath = obj._strFilePath;
			_hGlobal = obj._hGlobal;
			_width = obj._width;
			_height = obj._height;
			_x = obj._x;
			_y = obj._y;
			_cx = obj._cx;
			_cy = obj._cy;
		}
		return *this;
	}
};
//����ͼƬ��Ϣ
struct RenderInfo 
{
	RenderInfo(){
		_no = 8;//��ʼ��Ϊ8����һ��ʼ��ʾ��ͼƬΪ8
		_x = _y = 0;
	}
	unsigned int _no;		//Ҫ���Ƶ�ͼ�εı��
	OLE_XPOS_HIMETRIC _x;	//��Ⱦ��ͼ�ε���ʼ��x����
	OLE_YPOS_HIMETRIC _y;	//��Ⱦ��ͼ�ε���ʼ��y����
};
//������Ϣ
struct WinnerInfo
{
	WinnerInfo()
	{}
	WinnerInfo(const WinnerInfo& obj)
	{
		_class = obj._class;
		_name = obj._name;
		_award = obj._award;
	}
	WinnerInfo& operator = (const WinnerInfo& obj)
	{
		if (this != &obj)
		{
			_class = obj._class;
			_name = obj._name;
			_award = obj._award;
		}
		return *this;
	}
	std::wstring _class;	//��������
	std::wstring _name;	//�н�������
	std::wstring _award;	//��Ʒ
};
//��Ʒ������Ϣ
struct AwardInfo
{
	AwardInfo(){
		_tnum = _fnum = 0;
	}
	AwardInfo(const AwardInfo& obj)
	{
		_tnum = obj._tnum;
		_fnum = obj._fnum;
		_class = obj._class;
		_award = obj._award;
	}
	AwardInfo& operator = (const AwardInfo& obj)
	{
		if (this != &obj)
		{
			_tnum = obj._tnum;
			_fnum = obj._fnum;
			_class = obj._class;
			_award = obj._award;
		}
		return *this;
	}
	size_t		_tnum;	//��Ʒ����
	size_t		_fnum;	//�Ѿ���õĽ�Ʒ��
	std::wstring _class; //ʲô����һ�Ƚ�/���Ƚ�����
	std::wstring _award; //��Ʒ
};
//�����ߵ���Ϣ
struct PartnersInfo
{
	PartnersInfo()
	{
		_name = L"";
		_pn = L"";
	}
	PartnersInfo(const PartnersInfo& obj)
	{
		_name = obj._name;
		_pn = obj._pn;
	}
	PartnersInfo& operator = (const PartnersInfo& obj)
	{
		if (this != &obj)
		{
			_name = obj._name;
			_pn = obj._pn;
		}
		return *this;
	}
	bool operator == (const PartnersInfo& obj)
	{
		//���ͬ����ͬ����ô����������������ˣ��������������������ţ��������޷��ж����Ƿ�������
		//���������ͬ���������ף����ų�������ʵ����ͬ������ĳ��֣�
		return ((_pn.compare(obj._pn)==0) && (_name.compare(obj._name)==0));
	}
	bool operator != (const PartnersInfo& obj)
	{
		return !(*this==obj);
	}
	void clear(){
		_name = L"";
		_pn = L"";
	}
	std::wstring _name;	//����
	std::wstring _pn;	//�ֻ���
};
//��������ʾ����Ϣ
struct UiInfo
{
	std::wstring _title;		//����
	std::wstring _curClass;		//��ǰ��Ľ���
	std::wstring _curAward;		//��ǰ�Ľ���
	size_t		_tNum;			//��ǰ�����ܹ�������
	size_t		_fNum;			//��ǰ�����Ѿ���ȡ������
};

// CLotteryDlg �Ի���
class CLotteryDlg : public CDialog
{
// ����
public:
	CLotteryDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_LOTTERY_DIALOG };
	enum SOUNDTYPE{
		BEGIN = 0,		//��������
		LOTTERYING = 1, //���ڳ齱����ͼƬ������
		STOPPING = 2,	//����ֹͣ����ͼƬ������
		STOPPED = 3		//ֹͣ����ͼƬ������
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

public:
	//��ʼ���������
	void InitLottery();
	//����������Դ _loadPicΪimpl����
	void LoadDigitalPic(CString strFolderPath);
	//���ز�����ͼƬ
	
	//���ر���ͼƬ
	void LoadBgPictrue();
	//ȫ��
	void fullScreen();
	
private:
	//ʹ��OleLoadPicture�ķ�ʽ����ͼƬ
	bool _loadPic(CString strFullPath, PicInfo* picInfo);
	//�ͷ�IPictrue�õ�����Դ
	void _unloadPic();
	//���������
	void _genRandomNum();
	//������һ��ͼ����ʾ����Ϣ
	void _calcRenderInfo();
	//����ͼƬ��ʾ�������
	void _calcMargin();
	//��ʼ��һ������ĳ�ȡ����Ҫ���Ĳ���
	void _beginNextRing();
	//����һ���齱���ڣ���Ҫ���Ķ���
	void _finishRing();
	//���ƻ��û���ϸ��Ϣ
	void _drawWinnerInfo(CDC* pDC);
	//���Ƴ齱��Ϣ
	void _drawLotteryInfo(CDC* pDC);
	//������ʾ��Ϣ
	void _drawTips(CDC* pDC);
	//����ͼƬ
	void drawFrontPage(CDC* pDC);
	//��ȡ�����ļ�����
	void _readInit();
	//��ȡ��������Ϣ��������
	void _checkFinishedNum();
	//�����˳�֮��Ҫ����д�������ļ�
	void _writeInit();
	//ȡ�����ڽ��еĳ齱��Ϣ
	AwardInfo _getCurAwardInfo();
	//ȡ����һ��Ҫ���еĳ齱��Ϣ
	AwardInfo _getNextAwardInfo();
	//����Ƿ�����齱��
	bool _getFinishLottery();
	//����Ƿ��ǵ�һ�γ齱
	bool _getIsFirstLottery();
	//��������
	void _playSound(SOUNDTYPE st, bool bLoop);
	//ֹͣ����
	void _stopSound();
	//��ȡ������ͼƬ��ȫ���ļ�·��
	void _getAllPartnerPicPath();
	//�õ���һ���б�Ĳ����߱��
	void _getFirstListOfPartnerNo();
	//�õ���һ���б�Ĳ����߱��
	void _getNextListOfPartnerNo();
	//�õ�����б�����һ�������ߵı�ţ�ע������˵��������б��еı��
	size_t _getNextPartnerNo();
	//��ȡ��ҳͼƬ
	void _readFrontPagePic();
	//��ȡ������õ����б�m_RandNo���û���ŵ�ͼƬ
	void _readTheRandPartnerPic();
	//�ͷ�����õ����б�m_RandNo���û���ŵ�ͼƬ����Դ
	void releaseTheListOfPartnerPic();
	//����ĳһ��ѭ����ͼƬ��λ�ã��Լ�����ˢ�µ���Ϣ
	void calcNextPicInfoOfRing();
	//���н��ߵ�ͼƬ�����浽�н����б���
	void _placeWinnerIntoArray(std::wstring strFileName);
	//���ָ����ͼƬ�Ƿ������һ���н���
	bool _isWinner(std::wstring strPath);
	//�����õ��Ѿ��н��ߵ�ͼƬ·��
	void _parseWinnerPicPath(std::wstring& strData);

private:
	//��ȡ�û�����
	void _readInfo();
	//�õ���ǰ���������·��
	CString _getCurPath();
	//��ʼ������
	void initParams();

	//��ӡ��ǰ����������н��߱��
	CString print();
	

// ʵ��
protected:
	HICON		m_hIcon;

private:
	HBITMAP		m_hBgPic;				//����ͼƬ�ľ��

	int			m_PicWidth;				//��¼����ͼƬ�Ŀ��
	int			m_PicHeight;			//��¼����ͼƬ�ĸ߶�
	int			m_StopTime;				//��¼���¿ո�ʼֹͣ����֮��ʱ����ʱ����
	size_t		m_CurIndex;				//��ǰ�н��ߵı��
	BOOL		m_bRealStop;			//��ΪĿǰʹ������ʱֹͣ������������˼�¼�Ƿ����ֹͣ��TRUE�������ʱ��2��û�б�ɱ��ΪFALSE
	BOOL 		m_bRollTimer;			//��¼��ǰ�Ƿ��й���ͼƬ���г齱�ļ�ʱ��
	BOOL		m_bFirstFlash;			//��¼���ǵ�һ֡ͼƬ
	PicInfo m_PicInfo[DIGITALNUM];		//��ż���ͼƬ����Ϣ
	RenderInfo m_RenderInfo[PICNUM];	//�����ȾͼƬ����Ϣ������n��λ��Ӧ����ʾ�ĸ�ͼƬ
	CRect m_RenderRect;					//��ȾͼƬ�ľ�������
	CRect m_ClientRect;					//�ͻ�����С

	CDC m_memDC;
	CFont		m_NormalFont;			//��ͨ����
	int			m_TipsFontHeight;		//��¼��ʾ���ֵ������С
	CFont		m_TipsFont;				//��ʾ��������
	CFont		m_EffectFont;			//��Ч��������
	CFont		m_KeyFont;				//��Ŀ����
	CFont		m_TitleFont;			//���⣬˵�����͵�����
	std::set<size_t> m_RandNo;			//��������
	std::list<PartnersInfo> m_PartnerInfo;//�����ߵ���Ϣ
	std::vector<std::wstring> m_PartnerPicPath;//��¼�����ߵ�ͼƬ·��
	std::vector<std::wstring> m_WinnerPicPath;//��¼���ߵ�ͼƬ�ļ���
	PartnersInfo m_CurPartnerInfo;		//��¼��ǰ�����Ĳ����ߵ���Ϣ
	std::vector<WinnerInfo> m_WinnerInfo;	//��¼�н��ߵ���Ϣ
	std::vector<AwardInfo> m_AwardInfo;	//����������Ϣ
	int m_TotalClass;					//��¼һ�����Ƚ�
	UiInfo m_uiInfo;					//��¼UI��Ϣ

	//////////////////////////////////////////////////////////////////////////
	size_t		m_NumFinished;				//��¼�Ѿ���ɵĽ����ȡ������
	//HBITMAP m_hPicOfRing[PICNUMOFRING];	//��¼ÿȦ�齱��ȡ��ͼƬ�ľ��
	PicInfo m_FrontPageInfo;					//��¼��һҳ����Ϣ
	PicInfo m_PartnerPicInfo[PICNUMOFRING];	//��¼��ȡ������ͼƬ����Ϣ
	size_t	m_CurPartnerNoInRandList;	//��¼��ǰ�����ߵı��

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	int m_picNum;		//ͼ�������
	int m_innerWidth;	//ͼ����
	int m_leftWidth;	//ͼ������
	size_t m_UserNum;	//�û���Ŀ
public:
	void setLeftWidth(int width);
	void setRightWidth(int width);
	void setInnerWidth(int width);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
