
// CaptureScreen.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCaptureScreenApp:
// �йش����ʵ�֣������ CaptureScreen.cpp
//

#define	CONTROL_KEY_DOWN		0x8000 


class CCaptureScreenApp : public CWinApp
{
public:
	CCaptureScreenApp();

// ��д
	public:
	virtual BOOL InitInstance();
	virtual BOOL ProcessMessageFilter(int code, LPMSG lpMsg);
// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCaptureScreenApp theApp;