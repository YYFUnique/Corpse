
// ClipBoardMontor.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CClipBoardMontorApp:
// �йش����ʵ�֣������ ClipBoardMontor.cpp
//

class CClipBoardMontorApp : public CWinApp
{
public:
	CClipBoardMontorApp();

// ��д
	public:
	virtual BOOL InitInstance();
	virtual BOOL ExitInstance();
	ULONG_PTR hGdiPlusToken;
// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CClipBoardMontorApp theApp;