
// CorpseTools.h : CorpseTools Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CCorpseToolsApp:
// �йش����ʵ�֣������ CorpseTools.cpp
//

class CCorpseToolsApp : public CWinApp
{
public:
	CCorpseToolsApp();


// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CCorpseToolsApp theApp;
