
// ClientApp.h : ClientApp Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CClientAppApp:
// �йش����ʵ�֣������ ClientApp.cpp
//

class CClientAppApp : public CWinApp
{
public:
	CClientAppApp();


// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CClientAppApp theApp;
