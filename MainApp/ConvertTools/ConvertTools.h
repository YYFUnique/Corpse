
// ConvertTools.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CConvertToolsApp:
// �йش����ʵ�֣������ ConvertTools.cpp
//

class CConvertToolsApp : public CWinApp
{
public:
	CConvertToolsApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CConvertToolsApp theApp;