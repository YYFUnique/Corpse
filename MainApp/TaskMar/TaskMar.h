
// TaskMar.h : TaskMar Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CTaskMarApp:
// �йش����ʵ�֣������ TaskMar.cpp
//

class CTaskMarApp : public CWinApp
{
public:
	CTaskMarApp();


// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CTaskMarApp theApp;
