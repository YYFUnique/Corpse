
// NTPTime.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CNTPTimeApp:
// �йش����ʵ�֣������ NTPTime.cpp
//

class CNTPTimeApp : public CWinApp
{
public:
	CNTPTimeApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CNTPTimeApp theApp;