
// AccountEncryptTool.h : AccountEncryptTool Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CAccountEncryptToolApp:
// �йش����ʵ�֣������ AccountEncryptTool.cpp
//

class CAccountEncryptToolApp : public CWinApp
{
public:
	CAccountEncryptToolApp();


// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CAccountEncryptToolApp theApp;
