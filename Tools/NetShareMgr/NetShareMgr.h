
// NetShareMgr.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CNetShareMgrApp:
// �йش����ʵ�֣������ NetShareMgr.cpp
//

class CNetShareMgrApp : public CWinApp
{
public:
	CNetShareMgrApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CNetShareMgrApp theApp;