
// DriverTool.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CDriverToolApp:
// �йش����ʵ�֣������ DriverTool.cpp
//

class CDriverToolApp : public CWinApp
{
public:
	CDriverToolApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CDriverToolApp theApp;