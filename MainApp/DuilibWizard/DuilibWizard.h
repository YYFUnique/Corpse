
// DuilibWizard.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CDuilibWizardApp:
// �йش����ʵ�֣������ DuilibWizard.cpp
//

class CDuilibWizardApp : public CWinApp
{
public:
	CDuilibWizardApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CDuilibWizardApp theApp;