
// CrackUrl.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCrackUrlApp:
// �йش����ʵ�֣������ CrackUrl.cpp
//

class CCrackUrlApp : public CWinApp
{
public:
	CCrackUrlApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCrackUrlApp theApp;