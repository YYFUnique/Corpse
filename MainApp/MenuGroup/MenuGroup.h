
// MenuGroup.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMenuGroupApp:
// �йش����ʵ�֣������ MenuGroup.cpp
//

class CMenuGroupApp : public CWinApp
{
public:
	CMenuGroupApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMenuGroupApp theApp;