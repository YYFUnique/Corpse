
// IEHistory.h : IEHistory Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CIEHistoryApp:
// �йش����ʵ�֣������ IEHistory.cpp
//

class CIEHistoryApp : public CWinApp
{
public:
	CIEHistoryApp();


// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CIEHistoryApp theApp;
