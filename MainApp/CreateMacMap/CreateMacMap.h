
// CreateMacMap.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCreateMacMapApp:
// �йش����ʵ�֣������ CreateMacMap.cpp
//

class CCreateMacMapApp : public CWinApp
{
public:
	CCreateMacMapApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCreateMacMapApp theApp;