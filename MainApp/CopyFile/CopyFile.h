
// CopyFile.h : CopyFile Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CCopyFileApp:
// �йش����ʵ�֣������ CopyFile.cpp
//

class CCopyFileApp : public CWinApp
{
public:
	CCopyFileApp();


// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CCopyFileApp theApp;
