
// HotFix.h : HotFix Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CHotFixApp:
// �йش����ʵ�֣������ HotFix.cpp
//

class CHotFixApp : public CWinApp
{
public:
	CHotFixApp();


// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CHotFixApp theApp;
