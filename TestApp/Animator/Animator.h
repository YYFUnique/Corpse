
// Animator.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "Animator_i.h"
#include <GdiPlus.h>
using namespace Gdiplus;

// CAnimatorApp:
// �йش����ʵ�֣������ Animator.cpp
//

class CAnimatorApp : public CWinApp
{
public:
	CAnimatorApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
	BOOL ExitInstance(void);
	ULONG_PTR m_ulToken;
};

extern CAnimatorApp theApp;