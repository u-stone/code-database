
// LotteryDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Lottery.h"
#include "LotteryDlg.h"
#include <Mmsystem.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLotteryDlg 对话框
#pragma comment(lib, "Winmm.lib")


CLotteryDlg::CLotteryDlg(CWnd* pParent /*=NULL*/)
: CDialog(CLotteryDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hBgPic = NULL;
	m_PicWidth = 0;
	m_PicHeight = 0;
	m_leftWidth = 50;
	m_CurIndex = 0;
	m_bRealStop = TRUE;
	m_TipsFontHeight = 22;
	m_CurPartnerNoInRandList = 0;
	m_bFirstFlash = TRUE;
	m_NumFinished = 0;
}

void CLotteryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLotteryDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CLotteryDlg 消息处理程序

BOOL CLotteryDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	fullScreen();

	InitLottery();

	m_bRollTimer = FALSE;

	_playSound(BEGIN, true);
	SetFocus();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CLotteryDlg::OnPaint()
{
	if (IsIconic())
	{
	}
	else
	{
		CPaintDC hDC(this);
		CRect rect(m_RenderRect);
		GetClientRect(&rect);
		//rect.InflateRect(10, 10);

		//绘制数字图片
		int xPos = m_leftWidth, yPos = m_RenderRect.top;
		int index = 0;
		//int num = sizeof(m_RenderInfo)/sizeof(m_RenderInfo[0]);
		//for (int i = 0; i < num; ++i)
		//{
		//	index = m_RenderInfo[i]._no;
		//	HRESULT hRes = m_PicInfo[index]._iPic->Render(hDC, xPos, yPos, m_PicInfo[index]._width, m_PicInfo[index]._height, 
		//		m_PicInfo[index]._x, m_PicInfo[index]._cy, m_PicInfo[index]._cx, -m_PicInfo[index]._cy, NULL);
		//	xPos += m_PicWidth + m_innerWidth;
		//}
		try
		{
			//一次只显示一个图片
			index = m_CurPartnerNoInRandList;
			int picHeight = m_PartnerPicInfo[index]._height;
			int picWidth = m_PartnerPicInfo[index]._width;
			if (picWidth == 0)
				picWidth = 1;
			double ratio = double(picHeight) / double(picWidth);
			if (ratio < 0.001)
				ratio = 1.0;

			if (picHeight > rect.Height() * HEIGHTRATIO)//这里计算保证最大在指定的区域内
			{
				picHeight = int(rect.Height() * HEIGHTRATIO);
				picWidth = int(rect.Height() * HEIGHTRATIO / ratio);
			}
			yPos = rect.CenterPoint().y - picHeight/2;
			xPos = rect.CenterPoint().x - picWidth/2;
			if (!m_bFirstFlash)
			{
				HRESULT hRes = m_PartnerPicInfo[index]._iPic->Render(hDC, xPos, yPos-1, picWidth, picHeight, 
					m_PartnerPicInfo[index]._x, m_PartnerPicInfo[index]._cy, m_PartnerPicInfo[index]._cx, -m_PartnerPicInfo[index]._cy, NULL);
			}
			else
			{
				if (m_FrontPageInfo._iPic != NULL)
				{
					HRESULT hRes = m_FrontPageInfo._iPic->Render(hDC, xPos, yPos, picWidth, picHeight, 
						m_FrontPageInfo._x, m_FrontPageInfo._cy, m_FrontPageInfo._cx, -m_FrontPageInfo._cy, NULL);
				}
			}
		}
		catch(...)
		{
			return ;
		}
		SetTextAlign(hDC, TA_LEFT | TA_BOTTOM);
		//绘制界面信息
		_drawWinnerInfo(&hDC);
		_drawLotteryInfo(&hDC);
		_drawTips(&hDC);
		//print();

		CDialog::OnPaint();
	}
}
/*
HRESULT Render( 
HDC hdc, //Handle of device context on which to render the image
long x,  //Horizontal position of image in hdc
long y,  //Vertical position of image in hdc
long cx, //Horizontal dimension of destination rectangle
long cy, //Vertical dimension of destination rectangle
OLE_XPOS_HIMETRIC xSrc,
//Horizontal offset in source picture
OLE_YPOS_HIMETRIC ySrc,
//Vertical offset in source picture
OLE_XSIZE_HIMETRIC cxSrc,
//Amount to copy horizontally in source picture
OLE_YSIZE_HIMETRIC cySrc,
//Amount to copy vertically in source picture
LPCRECT prcWBounds
//Pointer to position of destination for a metafile hdc
);


*/

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CLotteryDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLotteryDlg::setLeftWidth(int width)
{
	m_leftWidth = width;
}
void CLotteryDlg::setInnerWidth(int width)
{
	m_innerWidth = width;
}


void CLotteryDlg::InitLottery()
{
	//初始化数据
	m_innerWidth = 10;
	m_picNum = PICNUM;
	m_UserNum = 0;
	initParams();
	LoadDigitalPic(L"F:\\pic\\第二套");
	LoadBgPictrue();
	//_readInfo();			//读取参与者数据
	_getAllPartnerPicPath();//读取参与者图片路径数据
	_readInit();			//读取配置文件
	_getFirstListOfPartnerNo();//得到第一个随机的用户列表
	_beginNextRing();		//开始这是得到第一圈的数据
	Invalidate();
}

void CLotteryDlg::LoadDigitalPic(CString strFolderPath)
{
	//CString strFilePath;
	//CPaintDC hDC(this);
	//m_PicWidth = 0;
	//m_PicHeight = 0;
	////加载图片
	//ZeroMemory(m_PicInfo, sizeof(PicInfo) * sizeof(m_PicInfo)/sizeof(m_PicInfo[0]) );
	//for (int i = 0; i < sizeof(m_PicInfo)/sizeof(m_PicInfo[0]); ++i)
	//{
	//	strFilePath.Format(L"\\%d.bmp", i);
	//	strFilePath = strFolderPath + strFilePath;
	//	if (!_loadPic(strFilePath, &m_PicInfo[i]))
	//	{
	//		AfxMessageBox(_T("加载图片:") + strFilePath + _T("出错"));
	//	}
	//	m_PicInfo[i]._width = MulDiv(m_PicInfo[i]._cx, GetDeviceCaps(hDC, LOGPIXELSX), 2540);
	//	m_PicInfo[i]._height = MulDiv(m_PicInfo[i]._cy, GetDeviceCaps(hDC, LOGPIXELSY), 2540);
	//	m_PicWidth += m_PicInfo[i]._width;
	//	if (m_PicHeight < m_PicInfo[i]._height)
	//		m_PicHeight = m_PicInfo[i]._height;
	//}
	//m_PicWidth /= sizeof(m_PicInfo)/sizeof(m_PicInfo[0]);
	//_calcMargin();	
}
bool CLotteryDlg::_loadPic(CString strFullPath, PicInfo* picInfo)
{
	try
	{
		HANDLE hFile = CreateFile(strFullPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return false;
		DWORD dwFileSize = GetFileSize(hFile, NULL);
		if (-1 == dwFileSize)
			return false;

		picInfo->_hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwFileSize);
		if (picInfo->_hGlobal == NULL)
			return false;

		void * pImageMemory = GlobalLock(picInfo->_hGlobal);
		DWORD dwReadedSize;//保存实际读取的文件的大小
		ReadFile(hFile, pImageMemory, dwFileSize, &dwReadedSize, NULL);
		GlobalUnlock(picInfo->_hGlobal);
		CloseHandle(hFile);
		IStream* pIStream;//创建一个ISTream接口指针，保存图片流
		IPicture* pIPicture;//创建一个IPicture接口指针，表示图片对象
		if (S_OK != CreateStreamOnHGlobal(picInfo->_hGlobal, false, &pIStream))//用全局内存初始化IStream接口指针
			return false;
		if (S_OK != OleLoadPicture(pIStream, 0, false, IID_IPicture, (LPVOID*)&(pIPicture)))//用OleLoadPicture获得IPicture接口指针
		{
			::GlobalFree(picInfo->_hGlobal);
			pIStream->Release();
			return false;
		}
		//得到IPicture COM接口对象后，就可以进行获得图片信息，显示图片等操作
		pIPicture->get_Width(&picInfo->_cx);
		pIPicture->get_Height(&picInfo->_cy);

		picInfo->_iStream = pIStream;
		picInfo->_iPic = pIPicture;
		picInfo->_x = picInfo->_y = 0;
		picInfo->_strFilePath = strFullPath;
	}
	catch (...)
	{
		return false;
	}
	return true;
}
void CLotteryDlg::_unloadPic()
{
	//try
	//{
	//	for (int i = 0; i < sizeof(m_PicInfo)/sizeof(m_PicInfo[0]); ++i)
	//	{
	//		::GlobalFree(m_PicInfo[i]._hGlobal);
	//		m_PicInfo[i]._iStream->Release();
	//		m_PicInfo[i]._iPic->Release();
	//	}
	//}
	//catch (...)
	//{
	//	AfxMessageBox(L"释放资源的时候出错");
	//}
}

void CLotteryDlg::_genRandomNum()
{
	if (sizeof(m_PartnerPicInfo)/sizeof(m_PartnerPicInfo[0]) > m_UserNum)
	{
		AfxMessageBox(L"抽奖人数比总人数还多！数据有误！");
		exit(0);
	}
	pair<std::set<unsigned int>::iterator, bool> res;
	size_t Num = 0;
	m_RandNo.clear();
	int maxLoop = 10000;
	int count = 0;
	try
	{
		for (size_t i = 0; i < sizeof(m_PartnerPicInfo)/sizeof(m_PartnerPicInfo[0]); )
		{
			Num = rand()%m_UserNum;
			count++;
			if (count > maxLoop)
			{
				//MessageBox(L"奖项过多，恭喜大家都有奖O(∩_∩)O~");
				//_writeInit();
				throw count;
			}
			if (!_isWinner(m_PartnerPicPath[Num]))//排除已经中奖者的可能
			{
				if (m_RandNo.insert(Num).second)//如果当前集合中也没有重复才可以i++
					++i;
			}
		}
	}
	catch (int)
	{
		MessageBox(L"奖项过多，恭喜大家都有奖O(∩_∩)O~");
		exit(0);
	}
}

void CLotteryDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 1)
	{
		//计算下一个图形显示的信息
		m_bRealStop = FALSE;//设置关
		//_calcRenderInfo();//计算参与者手机号的显示信息
		calcNextPicInfoOfRing();//计算参与者图片的显示信息
		InvalidateRect(m_RenderRect);
		PostMessage(WM_PAINT);
	}
	else if (nIDEvent == 2)
	{
		//计算下一个图形显示的信息
		static int _count = 0;//这里最大允许循环次数
		int times[8] = {100, 200, 300, 300, 400, 450, 500, 50};
		int stoptime = times[_count];
		CString str;
		str.Format(L"%d\n",stoptime);
		OutputDebugString(str);

		if (_count < sizeof(times)/sizeof(times[0])-1)//569*1.5=853.5
		{
			++_count;
			SetTimer(2, stoptime, NULL);
			//_calcRenderInfo();//算参与者手机号的显示信息
			calcNextPicInfoOfRing();//计算参与者图片的显示信息
			InvalidateRect(NULL);
			PostMessage(WM_PAINT);
			OutputDebugString(L"停止ing\n");
		}
		else
		{
			_count = 0;
			KillTimer(2);
			OutputDebugString(L"停止\n");
			m_bRealStop = TRUE;//设置开
			_finishRing();
			InvalidateRect(NULL);
			RedrawWindow();
			_getNextListOfPartnerNo();
			_playSound(BEGIN, true);
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CLotteryDlg::OnDestroy()
{
	CDialog::OnDestroy();
	//write back info into init file
	_writeInit();
	_unloadPic();
	releaseTheListOfPartnerPic();
}

void CLotteryDlg::_readInfo()
{
	CString strCurPath;
	if ((strCurPath = _getCurPath()).IsEmpty())
		return ;
	wchar_t strFilename[1024];
	memset(strFilename, 0, sizeof(strFilename));
	//MultiByteToWideChar(CP_ACP, 0, strCurPath, strCurPath.GetLength() * sizeof(TCHAR), strFilename, strCurPath.GetLength());
	wmemcpy_s(strFilename, sizeof(strFilename), (LPCTSTR)strCurPath, strCurPath.GetLength());
	std::wstring strDataPath(strFilename);

	strDataPath += L"data.txt";
	std::ifstream infile(strDataPath.c_str());
	if (!infile)
	{
		AfxMessageBox(L"没找到参与抽奖者数据");
		return ;
	}
	char buf[32];
	std::string strData;
	PartnersInfo pi;
	char separator[] = "\r\n\t ";
	bool bN = false, bP = false;
	while (1)
	{
		infile.getline(buf, sizeof(buf));
		strData = buf;
		size_t pos = 0;
		bN = false;
		for (; pos < strData.size(); ++pos)
		{
			if ((strData[pos] == '\t' || strData[pos] == ' '))
			{
				if (bN)
					break;
			}
			else
			{
				bN = true;
				pi._name += strData[pos];
			}
		}
		bP = false;
		for (; pos < strData.size(); ++pos)
		{
			if ((strData[pos] == '\t' || strData[pos] == ' '))
			{
				if (bP)
					break;
			}
			else
			{
				bP = true;
				pi._pn += strData[pos];
			}
		}
		if ((infile.rdstate() & ifstream::eofbit) != 0)
			break;
		//要排除数据中出现的信息重复情况
		if (find(m_PartnerInfo.begin(), m_PartnerInfo.end(), pi) == m_PartnerInfo.end())
			m_PartnerInfo.push_back(pi);
		pi.clear();
	}
	infile.close();
	infile.clear();
}
CString CLotteryDlg::_getCurPath()
{
	TCHAR szPath[1024] = {0};
	if (0 != GetModuleFileName(NULL, szPath, 1024))
	{
		CString str(szPath);
		str = str.Left(str.ReverseFind(L'\\') + 1);//"C:\Windows\System32\"这样的新形式
		return str;
	}
	return L"";
}
void CLotteryDlg::initParams()
{
	m_NormalFont.CreateFont(32,
		0,
		0,
		0,
		FW_NORMAL,
		0,
		0,
		0,
		GB2312_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		FF_DONTCARE,
		L"华文楷体"
		);
	m_KeyFont.CreateFont(42,
		0,
		0,
		0,
		FW_NORMAL,
		0,
		0,
		0,
		GB2312_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		FF_DONTCARE,
		L"华文楷体"
		);
	m_TitleFont.CreateFont(38,
		0,
		0,
		0,
		FW_NORMAL,
		0,
		0,
		0,
		GB2312_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		FF_DONTCARE,
		L"黑体"
		);
	m_TipsFont.CreateFont(m_TipsFontHeight,
		0,
		0,
		0,
		FW_NORMAL,
		0,
		0,
		0,
		GB2312_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		FF_DONTCARE,
		L"新宋体"
		);
	m_EffectFont.CreateFont(12,
		0,
		0,
		0,
		FW_BOLD,
		0,
		0,
		0,
		GB2312_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		FF_DONTCARE,
		L"新宋体"
		);
}
void CLotteryDlg::_calcRenderInfo()
{
	if (m_CurIndex >= m_PartnerInfo.size())//注意归零
		m_CurIndex = 0;
	//取出第index个参与者的姓名和手机号信息来
	size_t i = 0;
	//PartnersInfo m_CurPartnerInfo;
	for (std::list<PartnersInfo>::iterator iter = m_PartnerInfo.begin(); 
		i < m_PartnerInfo.size() && iter != m_PartnerInfo.end(); ++i, ++iter)
	{
		if (i == m_CurIndex)
		{
			m_CurPartnerInfo = *iter;
			break;
		}
	}
	//计算渲染信息
	for (size_t i = 0; i < sizeof(m_RenderInfo)/sizeof(m_RenderInfo[0]); ++i)
	{
		wchar_t dgt = m_CurPartnerInfo._pn[i];
		if (dgt >= '0' && dgt <= '9')
		{
			m_RenderInfo[i]._no = dgt - '0';
		}
	}
	++m_CurIndex;
}
void CLotteryDlg::_calcMargin()
{
	GetClientRect(&m_RenderRect);
	//m_innerWidth = (m_RenderRect.Width() - m_PicWidth * PICNUM - m_leftWidth * 2);
	//m_innerWidth /= PICNUM - 1;
	////if (m_innerWidth < 0)
	////	m_innerWidth = 0;
	//m_RenderRect.left = m_leftWidth;
	//m_RenderRect.right = m_RenderRect.left + m_PicWidth * m_PicWidth * PICNUM;
	//m_RenderRect.bottom = m_RenderRect.Height() / 2;
	//m_RenderRect.top = m_RenderRect.bottom - m_PicHeight;
	m_RenderRect.left = m_RenderRect.CenterPoint().x - m_PicWidth / 2;
	m_RenderRect.right = m_RenderRect.left + m_PicWidth;
	int height = m_RenderRect.Height();
	m_RenderRect.bottom = m_RenderRect.top + long(height * BOTTOMRATIO);
	m_RenderRect.top += long(height * TOPRATIO);
}
void CLotteryDlg::_beginNextRing()
{
	//如果已经抽完奖了，应该以一种什么方式退出？
	if (_getFinishLottery())
	{
		//AfxMessageBox(L"抽奖完毕");
		KillTimer(1);//杀死滚动图片的定时器，安全退出
		PostMessage(WM_QUIT);
	}
	AwardInfo ai = _getCurAwardInfo();
	//开始下一个奖品的抽取，这里要设置好相关的显示参数
	m_uiInfo._curAward = ai._award;//当前的奖品
	m_uiInfo._curClass = ai._class;//当前的奖项
	m_uiInfo._fNum = ai._fnum;		//已经抽取多少奖品
	m_uiInfo._tNum = ai._tnum;		//总共多少奖品
}
void CLotteryDlg::_finishRing()
{
	AwardInfo ai = _getCurAwardInfo();
	//当前中奖者，需要取消其再获奖资格
	_placeWinnerIntoArray(m_PartnerPicInfo[m_CurPartnerNoInRandList]._strFilePath);

	CString str;
	OutputDebugString(m_PartnerPicInfo[m_CurPartnerNoInRandList]._strFilePath.c_str());

	WinnerInfo wi;
	wi._class = ai._class;//什么等级的奖
	wi._name = m_CurPartnerInfo._name;
	wi._award = ai._award;//奖品
	m_WinnerInfo.push_back(wi);
	//完成一个奖项的抽取之后要设置接下来要的参数
	m_NumFinished++;//完成一个奖项的抽取，要将这个值加1
	ai = _getNextAwardInfo();
	//设置界面信息
	m_uiInfo._curAward = ai._award;
	m_uiInfo._curClass = ai._class;
	m_uiInfo._fNum = ai._fnum;		//已经抽取多少奖品
	m_uiInfo._tNum = ai._tnum;		//总共多少奖品
	//这里可以播放一段音乐
	//_playSound(STOPPED, false);
}
AwardInfo CLotteryDlg::_getCurAwardInfo()
{
	AwardInfo ai;
	for (std::vector<AwardInfo>::iterator iter = m_AwardInfo.begin(); iter != m_AwardInfo.end(); ++iter)
	{
		if (iter->_fnum < iter->_tnum)
		{
			ai = *iter;
			break;
		}
	}
	return ai;
}
AwardInfo CLotteryDlg::_getNextAwardInfo()
{
	AwardInfo ai;
	for (std::vector<AwardInfo>::iterator iter = m_AwardInfo.begin(); iter != m_AwardInfo.end(); ++iter)
	{
		if (iter->_fnum < iter->_tnum)
		{
			iter->_fnum++;
			ai = *iter;
			break;
		}
	}
	return ai;
}
bool CLotteryDlg::_getFinishLottery()
{
	for (std::vector<AwardInfo>::iterator iter = m_AwardInfo.begin(); iter != m_AwardInfo.end(); ++iter)
	{
		if (iter->_fnum < iter->_tnum)
		{
			return false;
			break;
		}
	}
	return true;
}
bool CLotteryDlg::_getIsFirstLottery()
{
	for (std::vector<AwardInfo>::iterator iter = m_AwardInfo.begin(); iter != m_AwardInfo.end(); ++iter)
	{
		if (iter->_fnum == 0)
			return true;
		else
			return false;
	}
	return false;
}
void CLotteryDlg::_playSound(SOUNDTYPE st, bool bLoop)
{
	//根据不同的类型播放不同的声音
	CString str = _getCurPath() + L"music\\";
	switch (st)
	{
	case BEGIN:
		str += L"begin.wav";
		break;
	case LOTTERYING:
		str += L"rolling.wav";
		break;
	case STOPPING:
		str += L"win.wav";
		break;
	case STOPPED:
		str += L"stop.wav";
		break;
	}
	if (bLoop)
		::sndPlaySound(str, SND_ASYNC | SND_LOOP);
	else
		::sndPlaySound(str, SND_ASYNC);
}
void CLotteryDlg::_stopSound()
{
	::sndPlaySound(NULL, SND_ASYNC);
}
void CLotteryDlg::_getAllPartnerPicPath()
{
	CFileFind cff;
	CString strPath = _getCurPath() + L"TMG_Worker_Photo\\*.jpg";

	BOOL bWorking = cff.FindFile(strPath);
	std::wstring strTmp;
	std::vector<std::wstring>::iterator iter;
	while (bWorking)
	{
		bWorking = cff.FindNextFile();
		strTmp = (LPCTSTR)cff.GetFilePath();
		if (find(m_PartnerPicPath.begin(), m_PartnerPicPath.end(), strTmp) == m_PartnerPicPath.end())
			m_PartnerPicPath.push_back(strTmp);
	}
	m_UserNum = m_PartnerPicPath.size();
	if (m_UserNum < PICNUMOFRING)
	{
		AfxMessageBox(L"数据图片太少");
		exit(0);
	}
}
void CLotteryDlg::_getFirstListOfPartnerNo()
{
	//m_WinnerPicPath.clear();
	_genRandomNum();
	_readFrontPagePic();
	_readTheRandPartnerPic();
}
void CLotteryDlg::_getNextListOfPartnerNo()
{
	OutputDebugString(L"计算下一次使用的图片\n");
	releaseTheListOfPartnerPic();
	_genRandomNum();
	_readTheRandPartnerPic();
}
size_t CLotteryDlg::_getNextPartnerNo()
{
	static size_t index = 0;
	index++;
	if (index >= m_RandNo.size())
		index = 0;
	CString str;
	str.Format(L"得到下一个参与者的编号为%d\n", index);
	OutputDebugString(str);
	return index;
}
void CLotteryDlg::_readFrontPagePic()
{
	ShowWindow(SW_HIDE);
	CString strFilePath = _getCurPath() + L"frontpage.bmp";
	CPaintDC hDC(this);
	m_PicWidth = 1;
	m_PicHeight = 1;
	//一次读取生成的随机数对应的编号中的字符串
	if (!_loadPic(strFilePath, &m_FrontPageInfo))
	{
		AfxMessageBox(_T("加载首页图片:") + strFilePath + _T("出错"));
		return ;
	}
	m_FrontPageInfo._width = MulDiv(m_FrontPageInfo._cx, GetDeviceCaps(hDC, LOGPIXELSX), 2540);
	m_FrontPageInfo._height = MulDiv(m_FrontPageInfo._cy, GetDeviceCaps(hDC, LOGPIXELSY), 2540);
	m_PicWidth += m_FrontPageInfo._width;
	if (m_PicHeight < m_FrontPageInfo._height)
		m_PicHeight = m_FrontPageInfo._height;
	_calcMargin();
	ShowWindow(SW_SHOW);
}
void CLotteryDlg::_readTheRandPartnerPic()
{
	CString strFilePath;
	CPaintDC hDC(this);
	m_PicWidth = 0;
	m_PicHeight = 0;
	int index = 0;
	//一次读取生成的随机数对应的编号中的字符串
	for (std::set<size_t>::iterator iter = m_RandNo.begin(); iter != m_RandNo.end(); ++iter)
	{
		strFilePath.SetString(m_PartnerPicPath[*iter].c_str());//设置读取的文件路径中的字符串
		if (!_loadPic(strFilePath, &m_PartnerPicInfo[index]))
		{
			AfxMessageBox(_T("加载图片:") + strFilePath + _T("出错"));
			continue;
		}
		m_PartnerPicInfo[index]._width = MulDiv(m_PartnerPicInfo[index]._cx, GetDeviceCaps(hDC, LOGPIXELSX), 2540);
		m_PartnerPicInfo[index]._height = MulDiv(m_PartnerPicInfo[index]._cy, GetDeviceCaps(hDC, LOGPIXELSY), 2540);
		m_PicWidth += m_PartnerPicInfo[index]._width;
		if (m_PicHeight < m_PartnerPicInfo[index]._height)
			m_PicHeight = m_PartnerPicInfo[index]._height;
		index++;
	}
	m_PicWidth /= sizeof(m_PartnerPicInfo)/sizeof(m_PartnerPicInfo[0]);
	_calcMargin();
}
void CLotteryDlg::releaseTheListOfPartnerPic()
{
	try
	{
		for (int i = 0; i < sizeof(m_PartnerPicInfo)/sizeof(m_PartnerPicInfo[0]); ++i)
		{
			::GlobalFree(m_PartnerPicInfo[i]._hGlobal);
			m_PartnerPicInfo[i]._iStream->Release();
			m_PartnerPicInfo[i]._iPic->Release();
		}
	}
	catch (...)
	{
		AfxMessageBox(L"释放资源的时候出错");
	}
}
void CLotteryDlg::calcNextPicInfoOfRing()
{
	m_CurPartnerNoInRandList = _getNextPartnerNo();

	static CRect oldRect = m_RenderRect;
	int width = m_PartnerPicInfo[m_CurPartnerNoInRandList]._width;
	m_RenderRect.left = m_RenderRect.CenterPoint().x - width / 2;
	m_RenderRect.right = m_RenderRect.left + m_PicWidth;
	//m_RenderRect.bottom = m_RenderRect.Height() / 2;
	//m_RenderRect.top = m_RenderRect.bottom - m_PicHeight;
	static CRect curRect;
	curRect = m_RenderRect;
	//计算得到一个最大的矩形区域，用作下一次刷新的范围
	if (curRect.left > oldRect.left) curRect.left = oldRect.left;
	if (curRect.top > oldRect.top)	curRect.top = oldRect.top;
	if (curRect.right < oldRect.right) curRect.right = oldRect.right;
	if (curRect.bottom < oldRect.bottom) curRect.bottom = oldRect.bottom;
	oldRect = m_RenderRect;
	m_RenderRect = curRect;//将计算得到的最大值赋值给新的矩形
}
void CLotteryDlg::_placeWinnerIntoArray(std::wstring strFilePath)
{
	m_WinnerPicPath.push_back(strFilePath);
}
bool CLotteryDlg::_isWinner(std::wstring strPath)
{
	return (find(m_WinnerPicPath.begin(), m_WinnerPicPath.end(), strPath) == m_WinnerPicPath.end())? false : true;
}
void CLotteryDlg::_parseWinnerPicPath(std::wstring& strData)
{
	std::wstring str;
	size_t off = 0;
	size_t begin = 0;
	size_t end = strData.find(L';', off);
	off = end+1;
	while (1)
	{
		str = strData.substr(begin, end-begin);
		if (str.empty())
			break;
		m_WinnerPicPath.push_back(str);
		begin = off;
		end = strData.find(L';', off);
		if (end == std::wstring::npos)
			break;
		off = end + 1;
	}
}
void CLotteryDlg::_drawWinnerInfo(CDC* pDC)
{
}
//抽奖信息的绘制
void CLotteryDlg::_drawLotteryInfo(CDC* pDC)
{
	pDC->SetBkMode(TRANSPARENT);
	CFont* pOldFont = pDC->SelectObject(&m_TitleFont);
	pDC->SetTextColor(RGB(255, 255, 255));
	//绘制标题
	CString str = L"天极传媒.上海2013迎春年会抽奖仪式---lgy";
	str.SetString(m_uiInfo._title.c_str());
	CSize size = pDC->GetTextExtent(str);
	pDC->TextOut(m_ClientRect.CenterPoint().x - size.cx/2, m_ClientRect.top + m_ClientRect.Height() / 8, str, str.GetLength());
	pDC->SelectObject(pOldFont);

	if (m_bRollTimer)
	{
		pDC->SetTextColor(RGB(255,255,0));
		pOldFont = pDC->SelectObject(&m_NormalFont);
		str = L"正在抽取";
		int xPos = m_ClientRect.left + m_leftWidth;
		int yPos = m_ClientRect.bottom - m_ClientRect.Height()/10;

		pDC->TextOut(xPos, yPos - 5, str, str.GetLength());
		xPos += pDC->GetTextExtent(str+L"123").cx;
		pDC->SelectObject(pOldFont);

		pOldFont = pDC->SelectObject(&m_KeyFont);
		str.SetString(m_uiInfo._curClass.c_str());//奖项
		pDC->TextOut(xPos, yPos, str, str.GetLength());

		str.SetString(m_uiInfo._curAward.c_str());//奖品
		str = L"奖品 : " + str;
		pDC->TextOut(m_ClientRect.CenterPoint().x, yPos, str, str.GetLength());
		pDC->SelectObject(pOldFont);
	}

}
void CLotteryDlg::drawFrontPage(CDC* pDC)
{
	if (!m_bFirstFlash)
		return ;

}
void CLotteryDlg::_drawTips(CDC* pDC)
{
	pDC->SetBkMode(TRANSPARENT);
	CFont* pOldFont = pDC->SelectObject(&m_TipsFont);
	CString str;
	if (m_bRollTimer)
		str = L"按下控制键停止";
	else//如果当前没有滚动图片进行抽奖的定时器
	{
		if (m_bRealStop)
		{
			str = L"按下控制键开始抽奖";
			if (!_getIsFirstLottery())
				str = L"按下控制键继续抽奖";
			if (_getFinishLottery())//没有在抽奖，并且已经完成了抽奖的数目
				str = L"抽奖结束，按下控制键退出";
		}
	}
	pDC->SetTextColor(RGB(255, 255, 255));
	pDC->TextOut(m_ClientRect.left + m_leftWidth, m_ClientRect.bottom - m_ClientRect.Height()/20, str, str.GetLength());

	if (m_bFirstFlash)
	{
		pDC->SelectObject(pOldFont);
		return ;
	}
	if (!_getFinishLottery())
	{
		pDC->SetTextColor(RGB(220, 220, 0));
		str.Format(L"当前奖项共设置%d人，已产生%d人，还有%d人", m_uiInfo._tNum, 
			m_uiInfo._fNum, m_uiInfo._tNum - m_uiInfo._fNum);
		if (m_uiInfo._tNum - m_uiInfo._fNum == 0)
			str.Format(L"当前奖项共设置%d人，已全部抽完", m_uiInfo._tNum);
		pDC->TextOut(m_ClientRect.CenterPoint().x, m_ClientRect.bottom - m_ClientRect.Height()/20, str, str.GetLength());
	}


	static int bCount = 8;
	if (!m_bRollTimer && !m_bRealStop && bCount > 0 && (m_uiInfo._tNum - m_uiInfo._fNum) != 0)
	{
		bCount--;
		if (bCount < 2)//如果抽取结束就
		{
			pDC->SetTextColor(RGB(50 + (2 - bCount) * 100, 50 + (2 - bCount) * 100, 50 + (2 - bCount) * 100));

			str.Format(L"当前奖项共设置%d人，已产生", m_uiInfo._tNum);//,  m_uiInfo._fNum, m_uiInfo._tNum - m_uiInfo._fNum);//%d人，还有%d人
			int cx1 = pDC->GetTextExtent(str).cx;

			str.Format(L"当前奖项共设置%d人，已产生%d人，还有", m_uiInfo._tNum, m_uiInfo._fNum);//%d人
			int cx2 = pDC->GetTextExtent(str).cx;

			str = L"+1";
			CFont* _pOldFont = pDC->SelectObject(&m_EffectFont);
			pDC->TextOut(m_ClientRect.CenterPoint().x + cx1, 
				m_ClientRect.bottom - m_ClientRect.Height()/20 - m_TipsFontHeight, str, str.GetLength());

			str = L"-1";
			pDC->TextOut(m_ClientRect.CenterPoint().x + cx2, 
				m_ClientRect.bottom - m_ClientRect.Height()/20 - m_TipsFontHeight, str, str.GetLength());
			pDC->SelectObject(_pOldFont);
		}
	}
	else if (m_bRealStop)
		bCount = 8;
	pDC->SelectObject(pOldFont);

}
void CLotteryDlg::_readInit()
{
	TCHAR path[MAX_PATH] = {0};
	CString strPath = _getCurPath() + _T("init.ini");
	GetPrivateProfileString(_T("Titile"), _T("title"), _T(""), path, MAX_PATH, strPath);
	CString str = path;
	m_uiInfo._title = str.GetBuffer();
	str.ReleaseBuffer();
	//读取已经中奖的人
	TCHAR *pBuf = new TCHAR[10240];
	memset(pBuf, 0, sizeof(TCHAR) * 10240);
	GetPrivateProfileString(_T("Procedure"), _T("winner"), _T(""), pBuf, 10240, strPath);
	std::wstring strwinner = pBuf;
	delete pBuf;
	pBuf = NULL;
	_parseWinnerPicPath(strwinner);
	//读取奖项设置
	m_TotalClass = GetPrivateProfileInt(_T("Adwards"), _T("total"), 3, strPath);
	m_NumFinished = GetPrivateProfileInt(_T("Procedure"), _T("finished"), 0, strPath);
	AwardInfo ai;
	CString tmp;
	int pos = 0;
	for (int i = m_TotalClass; i > 0; --i)
	{
		str.Format(_T("%d"), i);
		GetPrivateProfileString(_T("Adwards"), str, _T("0"), path, MAX_PATH, strPath);
		str = path;
		//几等奖品
		pos = str.Find(_T(' '));
		tmp = str.Left(pos);
		str = str.Right(str.GetLength() - pos);
		str.TrimLeft();
		ai._class = tmp.GetBuffer();
		tmp.ReleaseBuffer();
		//多少人的
		pos = str.Find(_T(' '));
		tmp = str.Left(pos-1);
		str = str.Right(str.GetLength() - pos);
		str.TrimLeft();
		ai._tnum = _wtoi(tmp.GetBuffer());
		tmp.ReleaseBuffer();
		//什么奖品
		str.Trim();
		ai._award = str.GetBuffer();
		str.ReleaseBuffer();

		m_AwardInfo.push_back(ai);
	}
	//读取信息后需要在做一下处理：比如说上次程序运行已经完成到什么阶段，现在应该接着运行
	_checkFinishedNum();
}
void CLotteryDlg::_checkFinishedNum()
{
	size_t tmp = m_NumFinished;
	for (std::vector<AwardInfo>::iterator iter = m_AwardInfo.begin(); iter != m_AwardInfo.end(); ++iter)
	{
		if (iter->_tnum <= tmp)//如果当前还有该奖项充足的分配额度，那么全部分配
		{
			iter->_fnum = iter->_tnum;
			tmp -= iter->_tnum;
		}
		else//如果没有当前级别充足的分配额度，那么分配全部分配之后退出
		{
			iter->_fnum = tmp;
			break;
		}
	}
}
void CLotteryDlg::_writeInit()
{
	size_t total = 0;
	for (std::vector<AwardInfo>::iterator iter = m_AwardInfo.begin(); iter != m_AwardInfo.end(); ++iter)
		total += iter->_tnum;
	if (m_NumFinished >= total)
		m_NumFinished = 0;//如果已经全部抽完了，要归零，而不是简单地写回值

	TCHAR path[MAX_PATH] = {0};
	CString strPath = _getCurPath() + _T("init.ini");
	CString str;
	str.Format(L"%d", m_NumFinished);
	WritePrivateProfileString(L"Procedure", L"finished", str, strPath);
	std::wstring strWinner = L"";
	if (m_NumFinished > 0)
	{
		for (std::vector<std::wstring>::iterator iter = m_WinnerPicPath.begin(); iter != m_WinnerPicPath.end(); ++iter)
			strWinner += *iter + L";";
	}
	WritePrivateProfileString(L"Procedure", L"winner", strWinner.c_str(), strPath);
}
BOOL CLotteryDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect rect;
	GetClientRect(&rect);
	if (!m_memDC.GetSafeHdc())
		m_memDC.CreateCompatibleDC(pDC);
	HGLOBAL hOld = m_memDC.SelectObject(m_hBgPic);
	pDC->BitBlt(0,0,rect.Width(), rect.Height(), &m_memDC, 0, 0, SRCCOPY);
	m_memDC.SelectObject(hOld);
	return FALSE;
	return CDialog::OnEraseBkgnd(pDC);
}


void CLotteryDlg::LoadBgPictrue()
{
	CString str = _getCurPath() + L"bg.bmp";
	CRect rect;
	GetClientRect(&rect);
	m_hBgPic = (HBITMAP)LoadImage(NULL, str, IMAGE_BITMAP, rect.Width(), rect.Height(), LR_LOADFROMFILE);
}
void CLotteryDlg::fullScreen()
{
	//设置全屏显示
	CRect m_RenderRect;
	GetClientRect(&m_RenderRect);
	int frameWidth = GetSystemMetrics(SM_CXFRAME);
	int frameHeight = GetSystemMetrics(SM_CYFRAME);
	int captionHeight = GetSystemMetrics(SM_CYCAPTION);
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	m_RenderRect.left -= frameWidth;
	m_RenderRect.top = m_RenderRect.top - frameHeight - captionHeight;
	m_RenderRect.bottom = m_RenderRect.top + screenHeight + 2 * frameHeight + captionHeight;
	m_RenderRect.right = m_RenderRect.left + screenWidth + 2 * frameWidth;
	ShowWindow(SW_HIDE);
	SetWindowPos(&wndTopMost, m_RenderRect.left, m_RenderRect.top, 
		m_RenderRect.Width(), m_RenderRect.Height(), SWP_HIDEWINDOW);
}
void CLotteryDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (m_hBgPic != NULL)
		DeleteObject(m_hBgPic);
	GetClientRect(&m_RenderRect);
	m_ClientRect = m_RenderRect;
	CString str = _getCurPath() + L"bg.bmp";
	m_hBgPic = (HBITMAP)LoadImage(NULL, str, IMAGE_BITMAP, m_RenderRect.Width(), m_RenderRect.Height(), LR_LOADFROMFILE);
	_calcMargin();
	InvalidateRect(NULL);
	PostMessage(WM_PAINT);
}

BOOL CLotteryDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_LBUTTONDOWN || pMsg->message == WM_RBUTTONDOWN || (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_SPACE))
	{
		static long ticktock = GetTickCount();
		//响应左键，右键，空格键
		if (GetTickCount() - ticktock > 1000)
		{
			m_bRollTimer = m_bRollTimer? FALSE : TRUE;
			if (m_bRollTimer)
			{
				if (m_bRealStop)
				{
					_stopSound();
					_beginNextRing();//开始一轮的抽奖
					m_bFirstFlash = FALSE;
					SetTimer(1, TIMER1, NULL);
					_playSound(LOTTERYING, true);
				}
				else
					m_bRollTimer = FALSE;
			}
			else
			{
				_stopSound();
				_playSound(STOPPING, false);
				KillTimer(1);
				m_StopTime = TIMER2;
				SetTimer(2, m_StopTime, NULL);
			}
			InvalidateRect(NULL);
			ticktock = GetTickCount();
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

CString CLotteryDlg::print()
{
	CString str = L"本次随机数", tmp;
	for (std::set<size_t>::iterator iter = m_RandNo.begin(); iter != m_RandNo.end(); ++iter)
	{
		tmp.Format(L"%d ", *iter);
		str += tmp;
	}
	str.Append(L"已中奖者");
	str += L"\n";
	size_t indexToAband = 0;
	size_t i = 0;
	for (std::set<size_t>::iterator iter = m_RandNo.begin(); iter != m_RandNo.end(); ++iter)
	{
		if (m_CurPartnerNoInRandList == i++)
		{
			indexToAband = *iter;
			break;
		}
	}
	tmp.Format(L"当前绘制的图片编号为%d\n", indexToAband);
	str += tmp;
	OutputDebugString(str);
	return str;
}