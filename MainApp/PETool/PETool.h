
// PETool.h : PETool Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CPEToolApp:
// �йش����ʵ�֣������ PETool.cpp
//

class CPEToolApp : public CWinApp
{
public:
	CPEToolApp();


// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CPEToolApp theApp;
