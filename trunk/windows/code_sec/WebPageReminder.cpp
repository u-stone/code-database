#include "StdAfx.h"
#include "WebPageReminder.h"
#include "WInternet.h"
#include <algorithm>

WebPageReminder* WebPageReminder::s_Obj = NULL;

WebPageReminder::WebPageReminder(void)
{
}

WebPageReminder::~WebPageReminder(void)
{
	for (std::vector<RefreshInfo*>::iterator iter = m_RefreshInfo.begin(); iter != m_RefreshInfo.end(); ++iter){
		delete *iter;
	}
}

UINT AFX_CDECL RefreshPage(LPVOID pParam){
	RefreshInfo info = *(RefreshInfo*)pParam;
	while (1){
		WInternet::GetFromWeb(info.strUrl);
		Sleep(120000);//休息2分钟
	}
}

void WebPageReminder::Refresh(CString strUrl)
{
	//防止对同一个url重复开启线程
	BOOL bFind = FALSE;
	for (std::vector<RefreshInfo*>::iterator iter = m_RefreshInfo.begin(); iter != m_RefreshInfo.end(); ++iter){
		if ((*iter)->strUrl.CompareNoCase(strUrl) == 0){
			bFind = TRUE;
			break;
		}
	}
	if (!bFind){
		RefreshInfo* pInfo = new RefreshInfo;
		m_RefreshInfo.push_back(pInfo);
		pInfo->strUrl = strUrl;
		m_RefreshInfo.push_back(pInfo);
		AfxBeginThread(RefreshPage, pInfo);
	}
}
