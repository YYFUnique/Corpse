
// NTPClient.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CNTPClientApp:
// �йش����ʵ�֣������ NTPClient.cpp
//

class CNTPClientApp : public CWinAppEx
{
public:
	CNTPClientApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CNTPClientApp theApp;