
// MakeSfx.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMakeSfxApp:
// �йش����ʵ�֣������ MakeSfx.cpp
//

class CMakeSfxApp : public CWinApp
{
public:
	CMakeSfxApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMakeSfxApp theApp;