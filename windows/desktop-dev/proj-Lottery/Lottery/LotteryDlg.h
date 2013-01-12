
// LotteryDlg.h : 头文件
//

#pragma once
#define PICNUM			11
#define DIGITALNUM		10
#define PICNUMOFRING	20	//定义每次循环读取的图片个数

#define TIMER1			100	//定义定时器1间隔时间
#define TIMER2			100	//定义定时器2间隔时间

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

//存放加载图片的信息
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
//数字图片信息
struct RenderInfo 
{
	RenderInfo(){
		_no = 8;//初始化为8，即一开始显示的图片为8
		_x = _y = 0;
	}
	unsigned int _no;		//要绘制的图形的编号
	OLE_XPOS_HIMETRIC _x;	//渲染该图形的起始点x坐标
	OLE_YPOS_HIMETRIC _y;	//渲染该图形的起始点y坐标
};
//获奖者信息
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
	std::wstring _class;	//奖项名称
	std::wstring _name;	//中奖者名字
	std::wstring _award;	//奖品
};
//奖品设置信息
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
	size_t		_tnum;	//奖品总数
	size_t		_fnum;	//已经抽好的奖品数
	std::wstring _class; //什么奖：一等奖/二等奖……
	std::wstring _award; //奖品
};
//参与者的信息
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
		//如果同名，同号那么就算是这个人作弊了，但是如果这个人用两个号，这样就无法判断其是否作弊了
		//但是如果，同名就算作弊，又排除不了现实中有同名现象的出现，
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
	std::wstring _name;	//姓名
	std::wstring _pn;	//手机号
};
//界面上显示的信息
struct UiInfo
{
	std::wstring _title;		//标题
	std::wstring _curClass;		//当前抽的奖项
	std::wstring _curAward;		//当前的奖项
	size_t		_tNum;			//当前奖项总共多少人
	size_t		_fNum;			//当前奖项已经抽取多少人
};

// CLotteryDlg 对话框
class CLotteryDlg : public CDialog
{
// 构造
public:
	CLotteryDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_LOTTERY_DIALOG };
	enum SOUNDTYPE{
		BEGIN = 0,		//开场音乐
		LOTTERYING = 1, //正在抽奖滚动图片的声音
		STOPPING = 2,	//正在停止滚动图片的声音
		STOPPED = 3		//停止滚动图片的声音
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:
	//初始化程序参数
	void InitLottery();
	//加载数字资源 _loadPic为impl函数
	void LoadDigitalPic(CString strFolderPath);
	//加载参与者图片
	
	//加载背景图片
	void LoadBgPictrue();
	//全屏
	void fullScreen();
	
private:
	//使用OleLoadPicture的方式加载图片
	bool _loadPic(CString strFullPath, PicInfo* picInfo);
	//释放IPictrue得到的资源
	void _unloadPic();
	//产生随机数
	void _genRandomNum();
	//计算下一次图形显示的信息
	void _calcRenderInfo();
	//计算图片显示相关数据
	void _calcMargin();
	//开始下一个奖项的抽取，需要做的操作
	void _beginNextRing();
	//结束一个抽奖环节，需要做的动作
	void _finishRing();
	//绘制获奖用户详细信息
	void _drawWinnerInfo(CDC* pDC);
	//绘制抽奖信息
	void _drawLotteryInfo(CDC* pDC);
	//绘制提示信息
	void _drawTips(CDC* pDC);
	//绘制图片
	void drawFrontPage(CDC* pDC);
	//读取配置文件数据
	void _readInit();
	//读取完配置信息后处理数据
	void _checkFinishedNum();
	//程序退出之后要重新写入配置文件
	void _writeInit();
	//取得正在进行的抽奖信息
	AwardInfo _getCurAwardInfo();
	//取得下一个要进行的抽奖信息
	AwardInfo _getNextAwardInfo();
	//检查是否结束抽奖了
	bool _getFinishLottery();
	//检查是否是第一次抽奖
	bool _getIsFirstLottery();
	//播放声音
	void _playSound(SOUNDTYPE st, bool bLoop);
	//停止播放
	void _stopSound();
	//读取参与者图片的全部文件路径
	void _getAllPartnerPicPath();
	//得到第一个列表的参与者编号
	void _getFirstListOfPartnerNo();
	//得到下一个列表的参与者编号
	void _getNextListOfPartnerNo();
	//得到随机列表中下一个参与者的编号，注意这里说的是随机列表中的编号
	size_t _getNextPartnerNo();
	//读取首页图片
	void _readFrontPagePic();
	//读取出随机得到的列表m_RandNo中用户编号的图片
	void _readTheRandPartnerPic();
	//释放随机得到的列表m_RandNo中用户编号的图片的资源
	void releaseTheListOfPartnerPic();
	//计算某一次循环中图片的位置，以及更新刷新的信息
	void calcNextPicInfoOfRing();
	//将中奖者的图片名保存到中奖者列表中
	void _placeWinnerIntoArray(std::wstring strFileName);
	//检查指定的图片是否代表着一个中奖者
	bool _isWinner(std::wstring strPath);
	//解析得到已经中奖者的图片路径
	void _parseWinnerPicPath(std::wstring& strData);

private:
	//读取用户数据
	void _readInfo();
	//得到当前程序的运行路径
	CString _getCurPath();
	//初始化参数
	void initParams();

	//打印当前的随机数和中奖者编号
	CString print();
	

// 实现
protected:
	HICON		m_hIcon;

private:
	HBITMAP		m_hBgPic;				//背景图片的句柄

	int			m_PicWidth;				//记录数字图片的宽度
	int			m_PicHeight;			//记录数字图片的高度
	int			m_StopTime;				//记录按下空格开始停止滚动之后定时器的时间间隔
	size_t		m_CurIndex;				//当前中奖者的编号
	BOOL		m_bRealStop;			//因为目前使用了延时停止，所以这个用了记录是否真的停止了TRUE，如果定时器2还没有被杀死为FALSE
	BOOL 		m_bRollTimer;			//记录当前是否有滚动图片进行抽奖的计时器
	BOOL		m_bFirstFlash;			//记录这是第一帧图片
	PicInfo m_PicInfo[DIGITALNUM];		//存放加载图片的信息
	RenderInfo m_RenderInfo[PICNUM];	//存放渲染图片的信息，即第n个位置应该显示哪个图片
	CRect m_RenderRect;					//渲染图片的矩形区域
	CRect m_ClientRect;					//客户区大小

	CDC m_memDC;
	CFont		m_NormalFont;			//普通字体
	int			m_TipsFontHeight;		//记录提示文字的字体大小
	CFont		m_TipsFont;				//提示文字字体
	CFont		m_EffectFont;			//特效文字字体
	CFont		m_KeyFont;				//醒目字体
	CFont		m_TitleFont;			//标题，说明类型的字体
	std::set<size_t> m_RandNo;			//存放随机数
	std::list<PartnersInfo> m_PartnerInfo;//参与者的信息
	std::vector<std::wstring> m_PartnerPicPath;//记录参与者的图片路径
	std::vector<std::wstring> m_WinnerPicPath;//记录获奖者的图片文件名
	PartnersInfo m_CurPartnerInfo;		//记录当前操作的参与者的信息
	std::vector<WinnerInfo> m_WinnerInfo;	//记录中奖者的信息
	std::vector<AwardInfo> m_AwardInfo;	//奖项设置信息
	int m_TotalClass;					//记录一共几等奖
	UiInfo m_uiInfo;					//记录UI信息

	//////////////////////////////////////////////////////////////////////////
	size_t		m_NumFinished;				//记录已经完成的奖项抽取的人数
	//HBITMAP m_hPicOfRing[PICNUMOFRING];	//记录每圈抽奖读取的图片的句柄
	PicInfo m_FrontPageInfo;					//记录第一页的信息
	PicInfo m_PartnerPicInfo[PICNUMOFRING];	//记录读取参与者图片的信息
	size_t	m_CurPartnerNoInRandList;	//记录当前参与者的编号

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	int m_picNum;		//图像的数量
	int m_innerWidth;	//图像间距
	int m_leftWidth;	//图组左间距
	size_t m_UserNum;	//用户数目
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
