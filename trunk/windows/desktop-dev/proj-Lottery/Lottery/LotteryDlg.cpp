
// LotteryDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Lottery.h"
#include "LotteryDlg.h"
#include <Mmsystem.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLotteryDlg �Ի���
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


// CLotteryDlg ��Ϣ�������

BOOL CLotteryDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	fullScreen();

	InitLottery();

	m_bRollTimer = FALSE;

	_playSound(BEGIN, true);
	SetFocus();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

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

		//��������ͼƬ
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
			//һ��ֻ��ʾһ��ͼƬ
			index = m_CurPartnerNoInRandList;
			int picHeight = m_PartnerPicInfo[index]._height;
			int picWidth = m_PartnerPicInfo[index]._width;
			if (picWidth == 0)
				picWidth = 1;
			double ratio = double(picHeight) / double(picWidth);
			if (ratio < 0.001)
				ratio = 1.0;

			if (picHeight > rect.Height() * HEIGHTRATIO)//������㱣֤�����ָ����������
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
		//���ƽ�����Ϣ
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

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
	//��ʼ������
	m_innerWidth = 10;
	m_picNum = PICNUM;
	m_UserNum = 0;
	initParams();
	LoadDigitalPic(L"F:\\pic\\�ڶ���");
	LoadBgPictrue();
	//_readInfo();			//��ȡ����������
	_getAllPartnerPicPath();//��ȡ������ͼƬ·������
	_readInit();			//��ȡ�����ļ�
	_getFirstListOfPartnerNo();//�õ���һ��������û��б�
	_beginNextRing();		//��ʼ���ǵõ���һȦ������
	Invalidate();
}

void CLotteryDlg::LoadDigitalPic(CString strFolderPath)
{
	//CString strFilePath;
	//CPaintDC hDC(this);
	//m_PicWidth = 0;
	//m_PicHeight = 0;
	////����ͼƬ
	//ZeroMemory(m_PicInfo, sizeof(PicInfo) * sizeof(m_PicInfo)/sizeof(m_PicInfo[0]) );
	//for (int i = 0; i < sizeof(m_PicInfo)/sizeof(m_PicInfo[0]); ++i)
	//{
	//	strFilePath.Format(L"\\%d.bmp", i);
	//	strFilePath = strFolderPath + strFilePath;
	//	if (!_loadPic(strFilePath, &m_PicInfo[i]))
	//	{
	//		AfxMessageBox(_T("����ͼƬ:") + strFilePath + _T("����"));
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
		DWORD dwReadedSize;//����ʵ�ʶ�ȡ���ļ��Ĵ�С
		ReadFile(hFile, pImageMemory, dwFileSize, &dwReadedSize, NULL);
		GlobalUnlock(picInfo->_hGlobal);
		CloseHandle(hFile);
		IStream* pIStream;//����һ��ISTream�ӿ�ָ�룬����ͼƬ��
		IPicture* pIPicture;//����һ��IPicture�ӿ�ָ�룬��ʾͼƬ����
		if (S_OK != CreateStreamOnHGlobal(picInfo->_hGlobal, false, &pIStream))//��ȫ���ڴ��ʼ��IStream�ӿ�ָ��
			return false;
		if (S_OK != OleLoadPicture(pIStream, 0, false, IID_IPicture, (LPVOID*)&(pIPicture)))//��OleLoadPicture���IPicture�ӿ�ָ��
		{
			::GlobalFree(picInfo->_hGlobal);
			pIStream->Release();
			return false;
		}
		//�õ�IPicture COM�ӿڶ���󣬾Ϳ��Խ��л��ͼƬ��Ϣ����ʾͼƬ�Ȳ���
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
	//	AfxMessageBox(L"�ͷ���Դ��ʱ�����");
	//}
}

void CLotteryDlg::_genRandomNum()
{
	if (sizeof(m_PartnerPicInfo)/sizeof(m_PartnerPicInfo[0]) > m_UserNum)
	{
		AfxMessageBox(L"�齱���������������࣡��������");
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
				//MessageBox(L"������࣬��ϲ��Ҷ��н�O(��_��)O~");
				//_writeInit();
				throw count;
			}
			if (!_isWinner(m_PartnerPicPath[Num]))//�ų��Ѿ��н��ߵĿ���
			{
				if (m_RandNo.insert(Num).second)//�����ǰ������Ҳû���ظ��ſ���i++
					++i;
			}
		}
	}
	catch (int)
	{
		MessageBox(L"������࣬��ϲ��Ҷ��н�O(��_��)O~");
		exit(0);
	}
}

void CLotteryDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (nIDEvent == 1)
	{
		//������һ��ͼ����ʾ����Ϣ
		m_bRealStop = FALSE;//���ù�
		//_calcRenderInfo();//����������ֻ��ŵ���ʾ��Ϣ
		calcNextPicInfoOfRing();//���������ͼƬ����ʾ��Ϣ
		InvalidateRect(m_RenderRect);
		PostMessage(WM_PAINT);
	}
	else if (nIDEvent == 2)
	{
		//������һ��ͼ����ʾ����Ϣ
		static int _count = 0;//�����������ѭ������
		int times[8] = {100, 200, 300, 300, 400, 450, 500, 50};
		int stoptime = times[_count];
		CString str;
		str.Format(L"%d\n",stoptime);
		OutputDebugString(str);

		if (_count < sizeof(times)/sizeof(times[0])-1)//569*1.5=853.5
		{
			++_count;
			SetTimer(2, stoptime, NULL);
			//_calcRenderInfo();//��������ֻ��ŵ���ʾ��Ϣ
			calcNextPicInfoOfRing();//���������ͼƬ����ʾ��Ϣ
			InvalidateRect(NULL);
			PostMessage(WM_PAINT);
			OutputDebugString(L"ֹͣing\n");
		}
		else
		{
			_count = 0;
			KillTimer(2);
			OutputDebugString(L"ֹͣ\n");
			m_bRealStop = TRUE;//���ÿ�
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
		AfxMessageBox(L"û�ҵ�����齱������");
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
		//Ҫ�ų������г��ֵ���Ϣ�ظ����
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
		str = str.Left(str.ReverseFind(L'\\') + 1);//"C:\Windows\System32\"����������ʽ
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
		L"���Ŀ���"
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
		L"���Ŀ���"
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
		L"����"
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
		L"������"
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
		L"������"
		);
}
void CLotteryDlg::_calcRenderInfo()
{
	if (m_CurIndex >= m_PartnerInfo.size())//ע�����
		m_CurIndex = 0;
	//ȡ����index�������ߵ��������ֻ�����Ϣ��
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
	//������Ⱦ��Ϣ
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
	//����Ѿ����꽱�ˣ�Ӧ����һ��ʲô��ʽ�˳���
	if (_getFinishLottery())
	{
		//AfxMessageBox(L"�齱���");
		KillTimer(1);//ɱ������ͼƬ�Ķ�ʱ������ȫ�˳�
		PostMessage(WM_QUIT);
	}
	AwardInfo ai = _getCurAwardInfo();
	//��ʼ��һ����Ʒ�ĳ�ȡ������Ҫ���ú���ص���ʾ����
	m_uiInfo._curAward = ai._award;//��ǰ�Ľ�Ʒ
	m_uiInfo._curClass = ai._class;//��ǰ�Ľ���
	m_uiInfo._fNum = ai._fnum;		//�Ѿ���ȡ���ٽ�Ʒ
	m_uiInfo._tNum = ai._tnum;		//�ܹ����ٽ�Ʒ
}
void CLotteryDlg::_finishRing()
{
	AwardInfo ai = _getCurAwardInfo();
	//��ǰ�н��ߣ���Ҫȡ�����ٻ��ʸ�
	_placeWinnerIntoArray(m_PartnerPicInfo[m_CurPartnerNoInRandList]._strFilePath);

	CString str;
	OutputDebugString(m_PartnerPicInfo[m_CurPartnerNoInRandList]._strFilePath.c_str());

	WinnerInfo wi;
	wi._class = ai._class;//ʲô�ȼ��Ľ�
	wi._name = m_CurPartnerInfo._name;
	wi._award = ai._award;//��Ʒ
	m_WinnerInfo.push_back(wi);
	//���һ������ĳ�ȡ֮��Ҫ���ý�����Ҫ�Ĳ���
	m_NumFinished++;//���һ������ĳ�ȡ��Ҫ�����ֵ��1
	ai = _getNextAwardInfo();
	//���ý�����Ϣ
	m_uiInfo._curAward = ai._award;
	m_uiInfo._curClass = ai._class;
	m_uiInfo._fNum = ai._fnum;		//�Ѿ���ȡ���ٽ�Ʒ
	m_uiInfo._tNum = ai._tnum;		//�ܹ����ٽ�Ʒ
	//������Բ���һ������
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
	//���ݲ�ͬ�����Ͳ��Ų�ͬ������
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
		AfxMessageBox(L"����ͼƬ̫��");
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
	OutputDebugString(L"������һ��ʹ�õ�ͼƬ\n");
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
	str.Format(L"�õ���һ�������ߵı��Ϊ%d\n", index);
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
	//һ�ζ�ȡ���ɵ��������Ӧ�ı���е��ַ���
	if (!_loadPic(strFilePath, &m_FrontPageInfo))
	{
		AfxMessageBox(_T("������ҳͼƬ:") + strFilePath + _T("����"));
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
	//һ�ζ�ȡ���ɵ��������Ӧ�ı���е��ַ���
	for (std::set<size_t>::iterator iter = m_RandNo.begin(); iter != m_RandNo.end(); ++iter)
	{
		strFilePath.SetString(m_PartnerPicPath[*iter].c_str());//���ö�ȡ���ļ�·���е��ַ���
		if (!_loadPic(strFilePath, &m_PartnerPicInfo[index]))
		{
			AfxMessageBox(_T("����ͼƬ:") + strFilePath + _T("����"));
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
		AfxMessageBox(L"�ͷ���Դ��ʱ�����");
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
	//����õ�һ�����ľ�������������һ��ˢ�µķ�Χ
	if (curRect.left > oldRect.left) curRect.left = oldRect.left;
	if (curRect.top > oldRect.top)	curRect.top = oldRect.top;
	if (curRect.right < oldRect.right) curRect.right = oldRect.right;
	if (curRect.bottom < oldRect.bottom) curRect.bottom = oldRect.bottom;
	oldRect = m_RenderRect;
	m_RenderRect = curRect;//������õ������ֵ��ֵ���µľ���
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
//�齱��Ϣ�Ļ���
void CLotteryDlg::_drawLotteryInfo(CDC* pDC)
{
	pDC->SetBkMode(TRANSPARENT);
	CFont* pOldFont = pDC->SelectObject(&m_TitleFont);
	pDC->SetTextColor(RGB(255, 255, 255));
	//���Ʊ���
	CString str = L"�켫��ý.�Ϻ�2013ӭ�����齱��ʽ---lgy";
	str.SetString(m_uiInfo._title.c_str());
	CSize size = pDC->GetTextExtent(str);
	pDC->TextOut(m_ClientRect.CenterPoint().x - size.cx/2, m_ClientRect.top + m_ClientRect.Height() / 8, str, str.GetLength());
	pDC->SelectObject(pOldFont);

	if (m_bRollTimer)
	{
		pDC->SetTextColor(RGB(255,255,0));
		pOldFont = pDC->SelectObject(&m_NormalFont);
		str = L"���ڳ�ȡ";
		int xPos = m_ClientRect.left + m_leftWidth;
		int yPos = m_ClientRect.bottom - m_ClientRect.Height()/10;

		pDC->TextOut(xPos, yPos - 5, str, str.GetLength());
		xPos += pDC->GetTextExtent(str+L"123").cx;
		pDC->SelectObject(pOldFont);

		pOldFont = pDC->SelectObject(&m_KeyFont);
		str.SetString(m_uiInfo._curClass.c_str());//����
		pDC->TextOut(xPos, yPos, str, str.GetLength());

		str.SetString(m_uiInfo._curAward.c_str());//��Ʒ
		str = L"��Ʒ : " + str;
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
		str = L"���¿��Ƽ�ֹͣ";
	else//�����ǰû�й���ͼƬ���г齱�Ķ�ʱ��
	{
		if (m_bRealStop)
		{
			str = L"���¿��Ƽ���ʼ�齱";
			if (!_getIsFirstLottery())
				str = L"���¿��Ƽ������齱";
			if (_getFinishLottery())//û���ڳ齱�������Ѿ�����˳齱����Ŀ
				str = L"�齱���������¿��Ƽ��˳�";
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
		str.Format(L"��ǰ�������%d�ˣ��Ѳ���%d�ˣ�����%d��", m_uiInfo._tNum, 
			m_uiInfo._fNum, m_uiInfo._tNum - m_uiInfo._fNum);
		if (m_uiInfo._tNum - m_uiInfo._fNum == 0)
			str.Format(L"��ǰ�������%d�ˣ���ȫ������", m_uiInfo._tNum);
		pDC->TextOut(m_ClientRect.CenterPoint().x, m_ClientRect.bottom - m_ClientRect.Height()/20, str, str.GetLength());
	}


	static int bCount = 8;
	if (!m_bRollTimer && !m_bRealStop && bCount > 0 && (m_uiInfo._tNum - m_uiInfo._fNum) != 0)
	{
		bCount--;
		if (bCount < 2)//�����ȡ������
		{
			pDC->SetTextColor(RGB(50 + (2 - bCount) * 100, 50 + (2 - bCount) * 100, 50 + (2 - bCount) * 100));

			str.Format(L"��ǰ�������%d�ˣ��Ѳ���", m_uiInfo._tNum);//,  m_uiInfo._fNum, m_uiInfo._tNum - m_uiInfo._fNum);//%d�ˣ�����%d��
			int cx1 = pDC->GetTextExtent(str).cx;

			str.Format(L"��ǰ�������%d�ˣ��Ѳ���%d�ˣ�����", m_uiInfo._tNum, m_uiInfo._fNum);//%d��
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
	//��ȡ�Ѿ��н�����
	TCHAR *pBuf = new TCHAR[10240];
	memset(pBuf, 0, sizeof(TCHAR) * 10240);
	GetPrivateProfileString(_T("Procedure"), _T("winner"), _T(""), pBuf, 10240, strPath);
	std::wstring strwinner = pBuf;
	delete pBuf;
	pBuf = NULL;
	_parseWinnerPicPath(strwinner);
	//��ȡ��������
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
		//���Ƚ�Ʒ
		pos = str.Find(_T(' '));
		tmp = str.Left(pos);
		str = str.Right(str.GetLength() - pos);
		str.TrimLeft();
		ai._class = tmp.GetBuffer();
		tmp.ReleaseBuffer();
		//�����˵�
		pos = str.Find(_T(' '));
		tmp = str.Left(pos-1);
		str = str.Right(str.GetLength() - pos);
		str.TrimLeft();
		ai._tnum = _wtoi(tmp.GetBuffer());
		tmp.ReleaseBuffer();
		//ʲô��Ʒ
		str.Trim();
		ai._award = str.GetBuffer();
		str.ReleaseBuffer();

		m_AwardInfo.push_back(ai);
	}
	//��ȡ��Ϣ����Ҫ����һ�´�������˵�ϴγ��������Ѿ���ɵ�ʲô�׶Σ�����Ӧ�ý�������
	_checkFinishedNum();
}
void CLotteryDlg::_checkFinishedNum()
{
	size_t tmp = m_NumFinished;
	for (std::vector<AwardInfo>::iterator iter = m_AwardInfo.begin(); iter != m_AwardInfo.end(); ++iter)
	{
		if (iter->_tnum <= tmp)//�����ǰ���иý������ķ����ȣ���ôȫ������
		{
			iter->_fnum = iter->_tnum;
			tmp -= iter->_tnum;
		}
		else//���û�е�ǰ�������ķ����ȣ���ô����ȫ������֮���˳�
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
		m_NumFinished = 0;//����Ѿ�ȫ�������ˣ�Ҫ���㣬�����Ǽ򵥵�д��ֵ

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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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
	//����ȫ����ʾ
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
	// TODO: �ڴ����ר�ô����/����û���
	if (pMsg->message == WM_LBUTTONDOWN || pMsg->message == WM_RBUTTONDOWN || (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_SPACE))
	{
		static long ticktock = GetTickCount();
		//��Ӧ������Ҽ����ո��
		if (GetTickCount() - ticktock > 1000)
		{
			m_bRollTimer = m_bRollTimer? FALSE : TRUE;
			if (m_bRollTimer)
			{
				if (m_bRealStop)
				{
					_stopSound();
					_beginNextRing();//��ʼһ�ֵĳ齱
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
	CString str = L"���������", tmp;
	for (std::set<size_t>::iterator iter = m_RandNo.begin(); iter != m_RandNo.end(); ++iter)
	{
		tmp.Format(L"%d ", *iter);
		str += tmp;
	}
	str.Append(L"���н���");
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
	tmp.Format(L"��ǰ���Ƶ�ͼƬ���Ϊ%d\n", indexToAband);
	str += tmp;
	OutputDebugString(str);
	return str;
}