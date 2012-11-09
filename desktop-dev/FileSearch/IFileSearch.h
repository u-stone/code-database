#pragma once
#include "stdafx.h"
#include <process.h>
#include <ShObjIdl.h>
#include <Shlwapi.h>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <list>
#include <algorithm>
using namespace std;

#include <cstdio>
class IFileSearch
{
protected:
	HWND m_hWnd;//搜索框的窗口句柄
public:
	//virtual void Exit() = 0;//退出搜索框
	virtual void InputStr(string pstr) = 0;//输入要搜索的文件名字符前缀
	virtual bool Select(int index) = 0;//选中搜索结果项
	virtual bool AddSearchFolder(string pAddedFolder) = 0;//添加搜索的文件夹
	virtual bool ExcludeSearchFolder(string pExcludeFolder) = 0;//添加不搜索的文件夹
	virtual bool GetResults(list<pair<string, string> >& sFilesSet) = 0;//返回搜索结果
	virtual HWND GetHWnd() = 0;//获得组合框的句柄
	virtual void ToSearch() = 0;//准备接收搜索字符串
	virtual string GetSelectedFilePath() = 0;//获得选中的文件路径字符串
};

extern "C" __declspec(dllexport) IFileSearch* GetFileSearcher(CWnd* pParentWnd);