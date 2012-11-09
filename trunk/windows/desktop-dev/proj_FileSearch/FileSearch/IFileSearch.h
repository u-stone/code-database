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
	HWND m_hWnd;//������Ĵ��ھ��
public:
	//virtual void Exit() = 0;//�˳�������
	virtual void InputStr(string pstr) = 0;//����Ҫ�������ļ����ַ�ǰ׺
	virtual bool Select(int index) = 0;//ѡ�����������
	virtual bool AddSearchFolder(string pAddedFolder) = 0;//����������ļ���
	virtual bool ExcludeSearchFolder(string pExcludeFolder) = 0;//��Ӳ��������ļ���
	virtual bool GetResults(list<pair<string, string> >& sFilesSet) = 0;//�����������
	virtual HWND GetHWnd() = 0;//�����Ͽ�ľ��
	virtual void ToSearch() = 0;//׼�����������ַ���
	virtual string GetSelectedFilePath() = 0;//���ѡ�е��ļ�·���ַ���
};

extern "C" __declspec(dllexport) IFileSearch* GetFileSearcher(CWnd* pParentWnd);